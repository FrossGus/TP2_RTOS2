/*
 * Task.h
 *
 *  Created on: May 19, 2019
 *      Author: julian
 */

#ifndef EXAMPLES_C_SAPI_RTOS_FREERTOS_STATIC_MEM_FREERTOS_01_BLINKY_INC_TASK_H_
#define EXAMPLES_C_SAPI_RTOS_FREERTOS_STATIC_MEM_FREERTOS_01_BLINKY_INC_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "queue.h"
#include "semphr.h"
#include "DriverDinamicMemoryRTOS.h"

extern SemaphoreHandle_t SemTxUart;
extern SemaphoreHandle_t SemRxUart;
extern Module_Data_t ModuleData;

void myTask_1( void* taskParmPtr );
void TaskTxUart( void* taskParmPtr );
void CallbackRx( void *noUsado );



#endif /* EXAMPLES_C_SAPI_RTOS_FREERTOS_STATIC_MEM_FREERTOS_01_BLINKY_INC_TASK_H_ */
