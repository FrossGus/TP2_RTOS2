/*
 * Task.c
 *
 *  Created on: May 19, 2019
 *      Author: julian
 */




#include "Task.h"


SemaphoreHandle_t SemTxUart;
SemaphoreHandle_t SemRxUart;
Module_Data_t ModuleData;
TaskHandle_t xTaskHandle_DL_RxNotify = NULL;
/*=================================================================================*/

void myTask_1( void* taskParmPtr )
{
	BaseType_t xStringNumber=0;
	BaseType_t xcopy;
	printf( "myTask_1\r\n" );

	gpioWrite( LED1, ON );
	// Envia la tarea al estado bloqueado durante 1 s (delay)
	vTaskDelay( 1000 / portTICK_RATE_MS );
	gpioWrite( LED1, OFF );

	// Tarea periodica cada 500 ms
	portTickType xPeriodicity =  500 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();

	while(TRUE) {
		//if( pdTRUE == xSemaphoreTake(SemRxUart, portMAX_DELAY) )
		{

			xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
			ModuleDinamicMemory_send(&ModuleData,0,NULL,"lala %d\r\n",xStringNumber,portMAX_DELAY,0);
			gpioToggle( LEDB );
			xStringNumber++;
			xSemaphoreGive(SemTxUart);
			// Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
			vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
		}
	}
}

/*===========Task receive==================================================================================*/
void TaskTxUart( void* taskParmPtr ){
	char * rx;
	printf( "TaskTxUart\r\n" );

	while(true){
		if( pdTRUE == xSemaphoreTake(SemTxUart,portMAX_DELAY) ){

			rx = ModuleDinamicMemory_receive(&ModuleData, portMAX_DELAY);
			printf( "rx %s\r\n",rx );
			ModuleDinamicMemory_Free(&ModuleData, rx);
		}
	}
}

void CallbackRx( void *noUsado ){
	volatile char buffer[30];
	static volatile uint8_t index = 0,ready = 0,startFrame = 0;
	UBaseType_t uxSavedInterruptStatus;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	char c = uartRxRead( UART_USB );

	if(c == '{') startFrame = 1;
	if(startFrame)	buffer[index++]= c;
	else return;

	if(index > sizeof(buffer)-1) index =0;
	buffer[index] = 0;
	if(c == '}'){
		startFrame = 0;
		ready = 1;
		xTaskNotifyFromISR(xTaskHandle_DL_RxNotify,0,eNoAction,xHigherPriorityTaskWoken);
		//ModuleDinamicMemory_send(&ModuleData,1,&xHigherPriorityTaskWoken, "lala %d\r\n",0,portMAX_DELAY,0);
		index =0;
		printf( "Recibimos <<%s>> por UART\r\n", buffer );
	}
	//printf( "Recibimos <<%c>> por UART\r\n", c );
	//if(ready == 1) xSemaphoreGiveFromISR( SemRxUart, &xHigherPriorityTaskWoken );

	if(xHigherPriorityTaskWoken) portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}



