/*
*  RTOS 2  - Grupo 1 - CESE 2019 
*  Gustavo, Jacobo, Julian, Rafael 
*  Buffer Circular de Salida
*  CircularBuffer.h
*/ 

#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include "sapi_datatypes.h"

#define LEN_BUFFER 600

typedef struct {
	uint8_t buffer[LEN_BUFFER];
	uint8_t	readbuf;
	uint8_t	writebuf;
} circularBuffer_t;

circularBuffer_t circularBuffer;

void init_buffer		( circularBuffer_t * circularBuffer );
bool read_buffer 		( circularBuffer_t * circularBuffer, uint8_t * data );
bool write_buffer 		( circularBuffer_t * circularBuffer, uint8_t data );
void write_bufferString	( circularBuffer_t * circularBuffer, char * str_ptr );

#endif
