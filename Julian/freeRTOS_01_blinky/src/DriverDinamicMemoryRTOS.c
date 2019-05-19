
#include "DriverDinamicMemoryRTOS.h"
#include "Task.h"
void ModuleDinamicMemory_initialize( Module_Data_t *obj , uint32_t MaxLength, xQueueSendFCN xQueueSendFCN,xQueueReceiveFCN xQueueReceiveFCN, xQueueCreateFCN xQueueCreateFCN, pvPortMallocFCN pvPortMallocFCN,vPortFreeFCN vPortFreeFCN)
{
	obj->xMaxStringLength = MaxLength;
   //obj->xPointerQueue = xQueueCreate( 2 , sizeof( char * ) ); /*cola punteros tipo char*/
	obj->xQueueCreateFunction =xQueueCreateFCN;
	obj->xPointerQueue = obj->xQueueCreateFunction( 2 , sizeof( char * ) , 0);
	obj->xQueueSendFunction = xQueueSendFCN;
	obj->xQueueReceiveFunction = xQueueReceiveFCN;
	obj->pvPortMallocFunction = pvPortMallocFCN;
	obj->vPortFreeFunction = vPortFreeFCN;
}

void ModuleDinamicMemory_send( Module_Data_t *obj , char* pbuf , uint32_t var,uint32_t portMaxDelay)
{
	//char* PcStringToSend = pvPortMalloc( obj->xMaxStringLength );
	char* PcStringToSend = obj->pvPortMallocFunction( obj->xMaxStringLength );
	 snprintf( PcStringToSend ,obj->xMaxStringLength, pbuf , var); /*lleno buffer a enviar con una variable*/
	 obj->xQueueSendFunction(obj->xPointerQueue ,&PcStringToSend,portMaxDelay, 0);
     //xQueueSend( obj->xPointerQueue ,&PcStringToSend,portMAX_DELAY );
}

char* ModuleDinamicMemory_receive(Module_Data_t *obj, uint32_t portMaxDelay){

	char* pbuffer; /*Dato recibido*/
	obj->xQueueReceiveFunction( obj->xPointerQueue , &pbuffer, portMaxDelay );
	//xQueueReceive( obj->xPointerQueue , &pbuffer, portMAX_DELAY );
	return pbuffer;
}

void ModuleDinamicMemory_Free(Module_Data_t *obj , char *ultimo_mensaje){
	obj->vPortFreeFunction(ultimo_mensaje);
	//vPortFree(ultimo_mensaje);
}
