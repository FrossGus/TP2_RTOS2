/*
*  RTOS 2  - Grupo 1 - CESE 2019 
*  Gustavo, Jacobo, Julian, Rafael 
*  Buffer Circular de Salida
*
*/ 


/*==================[includes]============================================*/

#include "sapi_datatypes.h"
#include "CircularBuffer.h"

/*==================[functions]=====================*/

void init_buffer( circularBuffer_t * circularBuffer )
{
	circularBuffer->writebuf = 0;
	circularBuffer->readbuf   = 0;
}

bool read_buffer (circularBuffer_t * circularBuffer, uint8_t * data)
{
	if ( circularBuffer->readbuf == circularBuffer->writebuf )
		return false;
	else {
		*data = circularBuffer->buffer[ circularBuffer->readbuf ];
		circularBuffer->readbuf	= ( circularBuffer->readbuf + 1 ) % LEN_BUFFER;
		return true;
	}
}

bool write_buffer (circularBuffer_t * circularBuffer, uint8_t data)		//char Data
{
	if ( (circularBuffer->writebuf + 1) % L_BUFFER == circularBuffer->readbuf )
		return false;											// Write buffer is full
	else {
		circularBuffer->buffer[ circularBuffer->writebuf ] = data;
		circularBuffer->writebuf = (circularBuffer->writebuf + 1) % L_BUFFER;
		return true;
	}
}

void write_bufferString (circularBuffer_t * circularBuffer, char* str_ptr)
{
	char i = 0;
	while( str_ptr [ i ] != '\0' )
	{
		write_buffer( circularBuffer, str_ptr [ i ] );
		i++;
	}
}
