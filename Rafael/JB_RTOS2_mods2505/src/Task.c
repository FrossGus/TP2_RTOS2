/*
 * Task.c
 *
 *  Created on: May 19, 2019
 *      Author: julian
 *  RTOS 2  - Grupo 1 - CESE 2019 
 *  Gustavo, Jacobo, Julian, Rafael 
 */




#include "Task.h"
#include "General.h"
#include "CircularBuffer.h"

/*Datos de Trama Recibida*/
volatile DataFrame_t Data;

/*Datos de trama para decodificar */
volatile Frame_parameters_t Frame_parameters = { '{',0 , {0,0}, NULL,NULL, '}' };

/*Semaforo Sincronizar UartTx*/
SemaphoreHandle_t SemTxUart;

/*Semaforo Sincronizar UartRx*/
SemaphoreHandle_t SemRxUart;

/*Semaforo Mutex proteger Uart*/
SemaphoreHandle_t SemMutexUart;

/*instanciar Driver memoria dinamica*/
Module_Data_t ModuleData;

/*Notificación para llegada de trama*/
TaskHandle_t xTaskHandle_RxNotify = NULL;

/*Puntero para crear la cola*/
QueueHandle_t xPointerQueue_OP0;

/*Puntero para crear la cola*/
QueueHandle_t xPointerQueue_OP1;

/*Puntero para crear la cola*/
QueueHandle_t xPointerQueue_3;


/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea  |
 =================================================================================*/

void TaskService( void* taskParmPtr )
{
	char *PtrSOF = NULL;
	char *PtrEOF = NULL;
	void* XPointerQueUe = NULL; /*Puntero auxiliar  a cola*/

	gpioWrite( LED1, ON );
	vTaskDelay( 1000 / portTICK_RATE_MS );
	gpioWrite( LED1, OFF );

	while(TRUE) {
		/*Notifica que llego trama Buena*/
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);

		/*Proteger datos para hacer copia local*/
		taskENTER_CRITICAL();
		Frame_parameters.BufferAux = ModuleData.pvPortMallocFunction(sizeof(Data.Buffer));
		strcpy((char*)Frame_parameters.BufferAux,(const char*)Data.Buffer);
		taskEXIT_CRITICAL();

		/*Buscar posición del inicio de la trama*/
		PtrSOF = strchr((const char*)Frame_parameters.BufferAux, Frame_parameters._SOF);

		if( PtrSOF != NULL ){
			/** Decodificar T :  T[0] -'0' *10 + T[1] - '0'*/
			Frame_parameters.T[0] =  ( *(PtrSOF +  OFFSET_TAMANO)-'0' )*10 + (*(PtrSOF +  OFFSET_TAMANO + 1)-'0' ) ;

			/** Decodificar OP */
			Frame_parameters.Operation = *(PtrSOF +  OFFSET_OP)-'0';

			/* Cantidad de memoria a reservar*/
			ModuleData.xMaxStringLength = Frame_parameters.T[0] + NUM_ELEMENTOS_REST_FRAME;

			//printf( "T %d\r\n",Frame_parameters.T[0]); // QUITAR
		}

		/*Selecionar operaacion*/
		XPointerQueUe = SelecQueueFromOperation(Frame_parameters.Operation);

		if(XPointerQueUe != NULL){
			/*Envía el puntero al buffer con la trama a la cola*/
			ModuleDinamicMemory_send(&ModuleData,0,NULL,(char*)Frame_parameters.BufferAux,XPointerQueUe ,portMAX_DELAY);
		}
		/*Libero memoria del buffer aux*/
		ModuleData.vPortFreeFunction(Frame_parameters.BufferAux );
		gpioToggle( LEDB );

		/*sincronizar-Permite transmitir por uart lo que se recibe por la cola*/
		xSemaphoreGive(SemTxUart);
	}
}

/*=================================================================================
 	 	 	 	 | Tarea Mayusculizar - 24.5.19 R.O. |
 =================================================================================*/
void Task_ToMayusculas_OP0( void* taskParmPtr ){
	char * rx;
	while(1){

		rx = ModuleDinamicMemory_receive(&ModuleData,xPointerQueue_OP0,  portMAX_DELAY);
		//PrintUartMessageMutex("Task_ToMayusculas_OP0", SemMutexUart);
		//PrintUartBuffMutex( "mayus %s\r\n",rx,SemMutexUart);
        // pasar string a mayusculas  
        packetToUpper(rx);
        // Enviar a cola de TaskTxUARt
        ModuleDinamicMemory_send(&ModuleData,0,NULL,rx, xPointerQueue_3,portMAX_DELAY);
		/*Libera memoria dinamica*/
		ModuleDinamicMemory_Free(&ModuleData, rx);
	}
}


/*=================================================================================
 	 	 	 	 | Tarea Minusculizar - 24.5.19 R.O. |
 ================================================================================= */
 
void Task_ToMinusculas_OP1( void* taskParmPtr ){
	char * rx;
	while(1){

		rx = ModuleDinamicMemory_receive(&ModuleData,xPointerQueue_OP1,  portMAX_DELAY);
		//PrintUartMessageMutex("Task_ToMinusculas_OP1", SemMutexUart);
		//PrintUartBuffMutex( "Minus %s\r\n",rx,SemMutexUart);
        packetToLower(rx);
        // Enviar a cola de TaskTxUARt
        ModuleDinamicMemory_send(&ModuleData,0,NULL,rx, xPointerQueue_3,portMAX_DELAY);
		/*Libera memoria dinamica*/
		ModuleDinamicMemory_Free(&ModuleData, rx);
	}
}


/*=================================================================================
 	 	  | Tarea Reportar stack disponible - 24.5.19 R.O.|
 =================================================================================*/
#ifdef OP2_ANDOP3_TASK_USED
void Task_ReportStack_OP2( void* taskParmPtr ){
	UBaseType_t uxHighWaterMark;
        char *rx;
        char tempStack[30];
	while(1){
 	    rx = ModuleDinamicMemory_receive(&ModuleData,xPointerQueue_OP2,  portMAX_DELAY);
        	    if(Frame_parameters.Operation ==OP0)
		{uxHighWaterMark = uxTaskGetStackHighWaterMark( &xTaskHandle_MayOP0);}
   	    else{
		uxHighWaterMark = uxTaskGetStackHighWaterMark(&xTaskHandle_MinOP1); 
                }   
	    memset(tempStack, 0, sizeof(tempStack ) );
	    sprintf(tempStack, "%d", uxHighWaterMark );
            strncpy(rx, tempStack, strlen(tempStack));         
            // Enviar a cola de TaskTxUARt
            ModuleDinamicMemory_send(&ModuleData,0,NULL,rx, xPointerQueue_3,portMAX_DELAY);
	    /*Libera memoria dinamica*/
	    ModuleDinamicMemory_Free(&ModuleData, rx);
        }


/*=================================================================================
 	 	 	 | Tarea Reportar heap disponible 24.5.19 R.O.|
void ModuleDinamicMemory_send( Module_Data_t *obj ,uint8_t Isr, long * const xHigherPriorityTaskWoken, char* pbuf ,char * XpointerQueue, uint32_t portMaxDelay){
	char* PcStringToSend = obj->pvPortMallocFunction( obj->xMaxStringLength );
	strcpy(PcStringToSend ,pbuf);

	//Si uso el enviar en una isr
	if(Isr) obj->xQueueSendFromISRFunction(XpointerQueue ,&PcStringToSend,xHigherPriorityTaskWoken, 0);
	else  obj->xQueueSendFunction(XpointerQueue ,&PcStringToSend,portMaxDelay, 0);
}			 
 =================================================================================*/
void Task_ReportHeap_OP3( void* taskParmPtr ){
	char *rx;
        char tempHeap[30];
	while(1){
 	    rx = ModuleDinamicMemory_receive(&ModuleData,xPointerQueue_OP3,  portMAX_DELAY);
	    memset(tempHeap, 0, sizeof(tempHeap) );
	    sprintf(tempHeap, "%d", xPortGetFreeHeapSize() );
            strncpy(rx, tempStack, strlen(tempStack));         
            // Enviar a cola de TaskTxUARt
            ModuleDinamicMemory_send(&ModuleData,0,NULL, rx, xPointerQueue_3,portMAX_DELAY);
	    /*Libera memoria dinamica*/
	    ModuleDinamicMemory_Free(&ModuleData, rx);
        }

}
#endif

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea tx - 25.05.19 |
 char* ModuleDinamicMemory_receive(Module_Data_t *obj ,char * XpointerQueue, uint32_t portMaxDelay){
 char* pbuffer;  //
 obj->xQueueReceiveFunction(XpointerQueue , &pbuffer, portMaxDelay );
 return pbuffer;
=================================================================================*/
void TaskTxUart( void* taskParmPtr ){
	char * rx;
	while(true){
			/*Recibe por la cola*/
			rx = ModuleDinamicMemory_receive(&ModuleData, xPointerQueue_3, portMAX_DELAY);
            PrintUartBuffMutex( "rxTsk: %s\r\n",rx,SemMutexUart);           
			/*Libera memoria dinamica*/
			ModuleDinamicMemory_Free(&ModuleData, rx);
            if( uartTxReady( UART_USB ) ){
				// La primera vez – con esto arranca
				Transmit_UART( 0 );
			}
		}
}


/*=================================================================================
 	 	 	 	 	 	 	 	 |Inicializador por ahora BufCirc - 25.05.19 |
 =================================================================================*/
void Task_Inicializar(void)
		{
	    char mensaje[100];

	    // 25.5.2019 Reemplazo printf()
		 init_buffer( &circularBuffer );
		 strncpy(mensaje,"Gustavo- Jacobo - Julian -Rafael",99);
		 //write_bufferString(mensaje);
		 print_to_circular("TP1_Final freeRTOS");
		 print_to_circular(mensaje);
		 if( uartTxReady( UART_USB ) ){
		 	// La primera vez hay que llamar a la función para que empiece a escribir
		 			Transmit_UART( 0 );
		 			}

		}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Callback IT RX |
 =================================================================================*/
void CallbackRx( void *noUsado ){

	UBaseType_t uxSavedInterruptStatus;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	volatile char c = uartRxRead( UART_USB );  /*Char received*/

	Add_IncommingFrame(uxSavedInterruptStatus ,xHigherPriorityTaskWoken,c);

	if(xHigherPriorityTaskWoken) portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


/*=================================================================================
 | Callback IT TX | - 25.5.2019 RO
  // Definidas en circularBuffer.h/.c
  //  	if( read_buffer( &circularBuffer, &Txbyte ) ){
 =================================================================================*/
void Transmit_UART ( void* noUsado )
{
	static int start_detected = 0;
	char Txbyte;
	if(read_buffer( &circularBuffer, &Txbyte ) ){
		uartTxWrite( UART_USB, Txbyte );
	}

}
