/*
 * Task.c
 *
 *  Created on: May 19, 2019
 *      Author: julian
 */




#include "Task.h"


xSemaphoreHandle SemTxUart;
Module_Data_t ModuleData;

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

		ModuleDinamicMemory_send(&ModuleData, "lala %d\r\n",xStringNumber,portMAX_DELAY);
		gpioToggle( LEDB );
		xStringNumber++;
		xSemaphoreGive(SemTxUart);
		// Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
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
