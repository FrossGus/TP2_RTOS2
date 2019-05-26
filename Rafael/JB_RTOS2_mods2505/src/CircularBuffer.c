/*
*  RTOS 2  - Grupo 1 - CESE 2019 
*  Gustavo, Jacobo, Julian, Rafael 
*  Buffer Circular de Salida
*
*/ 


/*==================[includes]============================================*/

#include "sapi_datatypes.h"
#include "CircularBuffer.h"

/*=========== Public Data ==========================*/

circularBuffer_t circularBuffer;     //- 26.5.2019 declare in .c,present as extern in .h

/*==================[functions]=====================*/


void init_buffer( circularBuffer_t * circBuffer )
{
	circBuffer->writebuf = 0;
	circBuffer->readbuf   = 0;
}

bool_t read_buffer(circularBuffer_t * circBuffer, uint8_t * data)
{
	if ( circBuffer->readbuf == circBuffer->writebuf )
		return false;
	else {
		*data = circBuffer-> buffer[circBuffer->readbuf ];
		circBuffer->readbuf	= ( circBuffer->readbuf + 1 ) % LEN_BUFFER;
		return true;
	}
}

uint8_t wr_buffer(circularBuffer_t * circBuffer, uint8_t data)
{
	if ( (circBuffer->writebuf + 1) % LEN_BUFFER == circBuffer->readbuf )
		return 0;											// Write buffer is full
	else {
		circBuffer->buffer[ circBuffer->writebuf ] = data;
		circBuffer->writebuf = (circBuffer->writebuf + 1) % LEN_BUFFER;
		return 1;
	}
}

void write_bufferString(circularBuffer_t * circBuffer, char* str_ptr)
{
#define MAX_LEN_STRING 300
	uint16_t i = 0;
	char *local_ptr = str_ptr;
	while( (*local_ptr != '\0')&&(i<MAX_LEN_STRING) )
	{
		wr_buffer(&circBuffer, *local_ptr );
		local_ptr++;
		i++;
	}
	wr_buffer(&circBuffer, '\0' ); // Null-terminate string
}
