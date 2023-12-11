/*
 * uart.c (ver2)
 *
 *      Author: nadin
 */

#include "./uart.h"

#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define BAUD_RATE 57600
//#define BAUD_RATE 19200

static int fd = -1;

static int uart_read( uint8_t* buf, size_t* size )
{
	int res = -1;
	const ssize_t r = read( fd, buf, *size );

	if( -1 == r )
		printf( "%s\n%s\nread ... %s\n", __FILE__, __FUNCTION__, strerror( errno ));
	else
	{
		if( *size != r )
			printf( "%s\n%s\nread read=%li size=%lu ... Warning\n", __FILE__, __FUNCTION__, r, *size );
		else
			res = 0;

		*size = r;
	}

	printf( "%s %lu\n", __FUNCTION__, *size );
	return res;
}

static int uart_write( uint8_t* buf, size_t* size )
{
	int res = -1;
	const ssize_t w = write( fd, buf, *size );

	if( -1 == w )
		printf( "%s\n%s\nwrite ... %s\n", __FILE__, __FUNCTION__, strerror( errno ));
	else
	{
		*size = w;

		if( *size != w )
			printf( "%s\n%s\nwrite write=%li size=%lu ... Warning\n", __FILE__, __FUNCTION__, w, *size );
		else
			res = 0;
	}

	printf( "%s %lu\n", __FUNCTION__, *size );
	return res;
}

static int uart_io( uint8_t* obuf, size_t* osize, uint8_t* ibuf, size_t* isize )
{
	int res = -1;
	struct timespec ts ={ tv_sec: 0, tv_nsec: 10000000 };

	if( 0 == uart_write( obuf, osize ))
	{
		nanosleep( &ts, NULL );

		if( 0 == uart_read( ibuf, isize ))
			res = 0;
	}

	return res;
}

static int deinit( void )
{
	close( fd );
	fd = -1;
	return 0;
}

static int init( const char* aName )
{
	int res = -1;

	if( -1 == fd )
		deinit();

	if( 0 > ( fd = open( aName, O_RDWR | O_NONBLOCK | O_NOCTTY | O_NDELAY )))
		printf( "open %s %s %i ... Error\n", aName, strerror( errno ), errno );
	else
	{
		struct termios t;

		if( 0 == tcgetattr( fd, &t ))
		{
			t.c_cflag |= CREAD;		// включить приёмник
			t.c_cflag |= CLOCAL;	// игнорировать управление линиями с помощью модема

			t.c_cflag &= ~CSIZE;	// очистить биты кол-во бит
			t.c_cflag |= CS8;		// кол-во бит 8
	#if 1
			t.c_cflag &= ~PARENB;	// запустить генерацию чётности при выводе и проверку четности на вводе
			t.c_cflag &= ~PARODD;	// установить нечетность на вводе и выводе
	#else
			t.c_cflag |= PARENB;	// запустить генерацию чётности при выводе и проверку четности на вводе
			t.c_cflag |= PARODD;	// установить нечетность на вводе и выводе
	#endif
			t.c_iflag = 0;
			t.c_oflag = 0;
			t.c_lflag = 0;

			t.c_ispeed = BAUD_RATE; /* input speed */
			t.c_ospeed = BAUD_RATE; /* output speed */

			cfsetspeed( &t, BAUD_RATE );

			printf( "Speed ... %i\n", BAUD_RATE );

			if( 0 == tcsetattr( fd, TCSANOW, &t ))
				if( 0 == tcflush( fd, TCIOFLUSH ))
					res = 0;
		}

		if( 0 != res )
			printf( "tcsetattr ... Error\n" ),
			close( fd ),
			fd = -1;
	}

	return res;
}

struct _uart_t uart =
{
	init  : init,
	deinit: deinit,
	read  : uart_read,
	write : uart_write,
	io    : uart_io
};
