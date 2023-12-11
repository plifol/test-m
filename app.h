/*
 * app.h (ver2)
 *
 *      Author: nadin
 */

#ifndef APP_H_
#define APP_H_

extern struct _app_t
{
	int( *const init )( void );
	int( *const run )( void );
	int( *const exch)( void);
	int( *const deinit )( void );
}
app;

#endif /* APP_H_ */
