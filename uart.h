/*
 * uart.h (ver2)
 *
 *      Author: nadin
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stddef.h>

extern struct _uart_t
{
	int( *const init   )( const char* aName );
	int( *const deinit )( void );
	int( *const read   )( uint8_t* buf , size_t* size );
	int( *const write  )( uint8_t* buf , size_t* size );
	int( *const io     )( uint8_t* obuf, size_t* osize, uint8_t* ibuf, size_t* isize );
}
uart;

#endif /* UART_H_ */
