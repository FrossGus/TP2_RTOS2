/* Copyright 2017-2018, Eric Pernia
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "stdlib.h"
// sAPI header
#include "sapi.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

DEBUG_PRINT_ENABLE;

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/
//Key status
	typedef struct{
			uint8_t sof;
			uint8_t op;
			uint8_t T;
			uint8_t *pData;
			uint8_t eof;
	}packet_t;

	typedef struct{
		packet_t  dataPacket;
		struct nodeList_t *next;
	}nodeList_t;



// Prototipo de funcion de la tarea
void myTaskKey( void* taskParmPtr );
void myTaskLed( void* taskParmPtr );
void insertNodeAtList(nodeList_t *ptrNodeAtList,packet_t *dataPacket);
void readNodeAtList(nodeList_t *ptrNodeAtList,packet_t *dataPacket);
void deleteNodeAtList(nodeList_t *ptrNodeAtList);


void packetToLower(uint8_t *ptrToPacketLower);
void packetToUpper(uint8_t *ptrToPacketUpper);

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void)
{
   // ---------- CONFIGURACIONES ------------------------------
   // Inicializar y configurar la plataforma
   boardConfig();

   // UART for debug messages
   debugPrintConfigUart( UART_USB, 115200 );
   debugPrintlnString( "Blinky con freeRTOS y sAPI." );

   // Led para dar se�al de vida
   //gpioWrite( LED3, ON );

// Crear tarea en freeRTOS
   xTaskCreate(
      myTaskKey,                     // Funcion de la tarea a ejecutar
      (const char *)"myTaskKey",     // Nombre de la tarea como String amigable para el usuario
      configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
      0,                          // Parametros de tarea
      tskIDLE_PRIORITY+1,         // Prioridad de la tarea
      0                           // Puntero a la tarea creada en el sistema
   );

// Crear tarea en freeRTOS
      xTaskCreate(
         myTaskLed,                  // Funcion de la tarea a ejecutar
         (const char *)"myTaskLed",  // Nombre de la tarea como String amigable para el usuario
         configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
         0,                          // Parametros de tarea
         tskIDLE_PRIORITY+1,         // Prioridad de la tarea
         0                           // Puntero a la tarea creada en el sistema
      );

   // Iniciar scheduler
   vTaskStartScheduler();

   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE ) {
      // Si cae en este while 1 significa que no pudo iniciar el scheduler
   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

// Implementacion de funcion de la tarea
void myTaskKey( void* taskParmPtr )
{
   // ---------- CONFIGURACIONES ------------------------------

   // Tarea periodica cada 500 ms
   portTickType xPeriodicity =  40 / portTICK_RATE_MS;
   portTickType xLastWakeTime = xTaskGetTickCount();
   
   // ---------- REPETIR POR SIEMPRE --------------------------
   while(TRUE) {

   // Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
   vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
   }
}

 void myTaskLed( void* taskParmPtr )
  {
      // ---------- CONFIGURACIONES ------------------------------
   	  uint8_t *ptrToPacket;

	  ptrToPacket=malloc(7*sizeof(uint8_t));

	  *ptrToPacket='{';
	  *(ptrToPacket+1)=1; //OP
	  *(ptrToPacket+2)=3;  //Tl
	  *(ptrToPacket+3)=0;   //Th
	  *(ptrToPacket+4)='H';
	  *(ptrToPacket+5)='o';
	  *(ptrToPacket+6)='L';
	  *(ptrToPacket+7)='}';

	  debugPrintlnString( "a minusculas" );
	  packetToLower(ptrToPacket);
	  debugPrintlnUInt(*(ptrToPacket+4));
	  debugPrintlnUInt(*(ptrToPacket+5));
	  debugPrintlnUInt(*(ptrToPacket+6));

	  debugPrintlnString( "a mayusculas" );
	  packetToUpper(ptrToPacket);
	  debugPrintlnUInt(*(ptrToPacket+4));
	  debugPrintlnUInt(*(ptrToPacket+5));
	  debugPrintlnUInt(*(ptrToPacket+6));

	  debugPrintlnString( "llegue aca" );
      // Tarea periodica cada 500 ms
      portTickType xPeriodicity =  40 / portTICK_RATE_MS;
      portTickType xLastWakeTime = xTaskGetTickCount();





      // ---------- REPETIR POR SIEMPRE --------------------------
      while(TRUE) {

      // Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
      vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
      }
}

/*==================[fin del archivo]========================================*/
 void packetToLower(uint8_t *ptrToPacketLower){

	 uint16_t tSizePacket;
	 uint8_t i;
	 tSizePacket=*(ptrToPacketLower+3)*256;
	 tSizePacket=tSizePacket+*(ptrToPacketLower+2);
	 for(i=0;i<tSizePacket;i++){
		 if(*(ptrToPacketLower+i+4)>=41 &&  *(ptrToPacketLower+i+4)<=90)
			 *(ptrToPacketLower+i+4)=*(ptrToPacketLower+i+4)+32;
	 }

 }

 void packetToUpper(uint8_t *ptrToPacketUpper){
	 uint16_t tSizePacket;
	 uint8_t i;
	 tSizePacket=*(ptrToPacketUpper+3)*256;
	 tSizePacket=tSizePacket+*(ptrToPacketUpper+2);
	 for(i=0;i<tSizePacket;i++){
	   if(*(ptrToPacketUpper+i+4)>=97 &&  *(ptrToPacketUpper+i+4)<=122)
	 	 *(ptrToPacketUpper+i+4)=*(ptrToPacketUpper+i+4)-32;
	  }
 }



 void insertNodeAtList(nodeList_t *ptrNodeAtList,packet_t *dataPacket){
/*	 static nodeList_t *ptrNodeCurrent=NULL,*ptrNodeLast;

	 if(ptrNodeAtList==NULL){
		 ptrNodeAtList=malloc(sizeof(nodeList_t));
		 ptrNodeAtList->dataPacket.sof=dataPacket->sof;
		 ptrNodeAtList->dataPacket.op=dataPacket->op;
		 ptrNodeAtList->dataPacket.T=dataPacket->T;
		 ptrNodeAtList->dataPacket.pData=NULL;
		 ptrNodeAtList->dataPacket.eof=dataPacket->eof;;
		 ptrNodeAtList->next=NULL;
		 ptrNodeCurrent=ptrNodeAtList;
	 }
	 else{
		 ptrNodeLast=malloc(sizeof(nodeList_t));
		 ptrNodeCurrent->next=ptrNodeLast;

	 }*/
 }
