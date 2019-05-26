/*
*  RTOS 2  - Grupo 1 - CESE 2019 
*  Gustavo, Jacobo, Julian, Rafael 
*  Buffer Circular de Salida
*  CircularBuffer.h
*/ 

#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

//#include "sapi_datatypes.h"
#include "sapi.h"

#define LEN_BUFFER 600

typedef struct {
	uint8_t buffer[LEN_BUFFER];
	uint8_t	readbuf;
	uint8_t	writebuf;
} circularBuffer_t;

extern circularBuffer_t circularBuffer; // 26.5.2019 declare in .c

void init_buffer( circularBuffer_t * circBuffer );
bool_t read_buffer( circularBuffer_t * circBuffer, uint8_t * data );

uint8_t wr_buffer(circularBuffer_t * circBuffer, uint8_t data );

void write_bufferString( circularBuffer_t * circBuffer, char * str_ptr );

#endif
