
// Includes de FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include  "General.h"

// sAPI header
#include "sapi.h"
#include "Task.h"
#include "semphr.h"
#include "sapi_uart.h"


DEBUG_PRINT_ENABLE;

int main(void){

	boardConfig();
	debugPrintConfigUart( UART_USB, 115200 );
	debugPrintlnString( "TP1_Final freeRTOS Julian-Jacobo -Rafael-Gustavo" );


	/*=======Config Uart===================================*/
	uartConfig(UART_USB, 115200);
	/*Callback interrupt*/
	uartCallbackSet(UART_USB, UART_RECEIVE, CallbackRx, NULL);
	/*Habilito todas las interrupciones de UART_USB*/
	uartInterrupt(UART_USB, true);


		//Requerimiento 2.5 – Transmision x buffer vacio
	//uartTxInterruptCallbackSet( UART_USB, Transmit_UART );
	//uartTxInterruptSet( UART_USB, true );

	SemTxUart 	 =  xSemaphoreCreateBinary();
	SemMutexUart =	xSemaphoreCreateMutex() ;

	xPointerQueue_OP0	= xQueueCreate(2 , sizeof(char *)); /*Create queue OP0*/
	xPointerQueue_OP1	= xQueueCreate(2 , sizeof(char *)); /*Create queue OP0*/
	xPointerQueue_3		= xQueueCreate(1 , sizeof(char *)); /*Create queue OP0*/


	xTaskCreate(TaskTxUart, (const char *)"TaskTxUart",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(TaskService, (const char *)"TaskService",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 2, &xTaskHandle_RxNotify);
	xTaskCreate(Task_ToMayusculas_OP0, (const char *)"Task_ToMayusculas_OP0",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(Task_ToMinusculas_OP1, (const char *)"Task_ToMinusculas_OP1",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);
	//xTaskCreate(Task_ReportStack_OP2, (const char *)"Task_ToMayusculas_OP2",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);
	//xTaskCreate(Task_ReportHeap_OP3, (const char *)"Task_ToMinusculas_OP3",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);

	/*Inicializar Driver memoria dinamica*/
	ModuleDinamicMemory_initialize(&ModuleData,50,xQueueGenericSend,xQueueGenericSendFromISR, xQueueReceive,xQueueGenericCreate, pvPortMalloc, vPortFree);

	/* Iniciar scheduler*/
	vTaskStartScheduler();

	while( TRUE ) {
	}

	return 0;
}
