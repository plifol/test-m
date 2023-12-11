/*
 * main.c (ver2)
 *
 *      Author: nadin
 */

#include "./app.h"

#include <stdio.h>

int main( int argc, char** argv )
{
	printf( "\ntest-m ... Entry\n" );

	app.init();
	app.run();

	printf( "test-m... Done\n" );
	return 0;
}
