/*
 * app.c (ver2)
 *
 *      Author: Nadin
 */

#include "./app.h"
#include "./uart.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Dialog.h>

#define DISPLAY_IP "127.0.0.1"
#define DISPLAY_PRM "0.0"

#define APP_WIDTH	1000
#define APP_HEIGHT	940

#define START_WR   ( 1 << 0 )
#define START_RD   ( 1 << 1 )
#define START_CNTR ( 1 << 2 )
#define START_ALL  ( 1 << 3 )

#define START		1
#define FINISH		2
#define BEG_TEST	3
#define END_TEST	4

#define COM1        1
#define COM2        2
#define COM3        3

#define STR_OK		"Ok\n"
#define STR_ERROR	"Ошибка\n"
#define STR_EXCH    "Обмен\n"

XtAppContext AppContext;
static Widget Desktop;

static Boolean IsStop = True;

static Widget TextOut1;
static Widget TextOut2;
static Widget CommandWidgetInit;
static Widget Data1_1, Data1_2, Data1_3, Data1_4, Data1_5, Data1_6, Data1_7, Data1_8;
static Widget Data2_1, Data2_2, Data2_3, Data2_4, Data2_5, Data2_6, Data2_7, Data2_8;
static Widget CCM1, CCM2, CCM3, CCM4;
static Widget W1IS11, W1IS12, W1IS13, W1IS14;
static Widget W1IS21, W1IS22, W1IS23, W1IS24;
static Widget W2IS11, W2IS12, W2IS13, W2IS14;
static Widget W2IS21, W2IS22, W2IS23, W2IS24;
static Widget CommandWidgetData;
static Widget InputDialog;
static Widget CommandWidgetWrData;
static Widget CommandWidgetWrCycle;
static Widget CommandWidgetRdData;
static Widget CommandWidgetRdCycle;
static Widget CommandWidgetCntrLine;
static Widget CommandWidgetCycle;
static Widget CommandWidgetCntrCycle;

static unsigned data_strus1_1, data_strus1_2, data_strus1_3, data_strus1_4, data_strus1_5, data_strus1_6, data_strus1_7, data_strus1_8;
static unsigned data_strus2_1, data_strus2_2, data_strus2_3, data_strus2_4, data_strus2_5, data_strus2_6, data_strus2_7, data_strus2_8;

static uint8_t out_buf[ 35 ];

static char mass    [ 8 ];
static char mass_res[ 5 ];
static char mass_s  [ 8 ];
static char mass_ss [ 4 ];

static size_t coun_byte_wr;
static size_t coun_byte_rd;

static uint8_t wr_buf [ 43 ];
static uint8_t rd_buf [ 43 ];

static unsigned num_start = 0U;

XEvent event;
	event.type = ClientMessage;
	event.xclient.send_event = True;
	event.xclient.format = 8;
	event.xclient.data.s[ 0 ] = START;

static __attribute__(( __unused__ ))
void dec_to_bin ( int dec )
{
	int i;
	int j;

	for ( i = 0; i < 8; i++)
		mass_s[i] = 0;

	i = 0;

	while ( dec != 0 )
	{
		if ( dec % 2 == 0)
			mass_s[i] = 0;
		else
			mass_s[i] = 1;
		dec = dec >> 1;
		i++;
	}

	for ( i = 7; i >=0; i--)
		printf ("%d", mass_s [i]);
	for( i = 0, j = 3; i < 4; i++, j-- )
		mass_ss[i] = mass_s[j];
	printf ( " " );
	for (i = 0; 4 > i; i++ )
		printf ( "%d", mass_ss[i] );

}

static __attribute__(( __unused__ ))
void dec_to_bin_1 ( const int dec, const int dec_op )
{
	int i;
	int j;
	int dec_res;

	if ( dec > dec_op )
		dec_res = dec - dec_op;
	else
		dec_res = dec + 256 - dec_op;
	for ( i = 0; i < 8; i++)
		mass[i] = 0;
	i = 0;

	while ( dec_res != 0 )
	{
		if ( dec_res % 2 == 0)
			mass[i] = 0;
		else
			mass[i] = 1;
		dec_res = dec_res >> 1;
		i++;
	}

	for( i = 0, j = 7; j >=3; i++, j-- )
		mass_res[i] = mass[j];
}

static void AddText( Widget aWidget, char* aText )
{
	XawTextBlock text =
	{
		firstPos: 0,
		/*format: XawFmt8Bit_C,*/
		ptr: aText,
		length: strlen( aText )
	};

	XtVaSetValues ( aWidget, XtNeditType, XawtextEdit, ( void* )0 );
	XawTextPosition pos = XawTextGetInsertionPoint( aWidget );
	XawTextReplace( aWidget, pos, pos + text.length, &text );
	XawTextSetInsertionPoint( aWidget, pos + text.length );
	XtVaSetValues ( aWidget, XtNeditType, XawtextRead, ( void* )0 );
}

static void AddTextPoint( Widget aWidget, char* aText )
{
	char str[ NAME_MAX + 1 ];
	int i;

	snprintf( str, NAME_MAX, "%s", aText );

	for( i = strlen( str ); 50 > i; ++i )
		strcat( str, "." );

	AddText( aWidget, str );
}

static Widget app_init( void )
{
	Widget app, form;

	XtToolkitInitialize();

	String res[] =
	{
		"app*font : -misc-liberation sans-medium-r-normal--0-0-0-0-p-0-koi8-r",
		( String )0,
	};

	int argc = 3;
	char dsp_prm[ NAME_MAX + 1 ];
	snprintf( dsp_prm, NAME_MAX, "%s:%s", DISPLAY_IP, DISPLAY_PRM );
	String argv[ 3 ] = { "Test M", "-display", dsp_prm };

	app = XtAppInitialize( &AppContext, "app", ( XrmOptionDescList )0, 0, &argc, argv, res, ( ArgList )0, 0 );

	if( app )
	{
		form = XtVaCreateManagedWidget(( _XtString )0, formWidgetClass, app,
				XtNwidth,			APP_WIDTH,
				XtNheight,			APP_HEIGHT,
				XtNborderWidth,		0,
				XtNhorizDistance,	0,
				XtNvertDistance,	0,
				( void* )0 );

		if( form )
			XtRealizeWidget( app );
	}

	return( Desktop = form );
}

static void ExitCallbackProc( Widget widget, XtPointer closure, XtPointer call_data )
{
	app.deinit();
}

static void CancelCallbackProc( Widget widget, XtPointer closure, XtPointer call_data )
{
	XtDestroyWidget( InputDialog );
}

static void OkCallbackProc( Widget widget, XtPointer closure, XtPointer call_data )
{
	char value[ PATH_MAX ];
	sprintf( value, "%s", XawDialogGetValueString( InputDialog ));
	XtDestroyWidget( InputDialog );
	uart.init( value );
	AddText( TextOut1, STR_OK );
}

static void InitUartCallbackProc( Widget widget, XtPointer closure, XtPointer call_data )
{
	InputDialog = XtVaCreateManagedWidget(( _XtString )0, dialogWidgetClass, Desktop,
			XtNx,     100,
			XtNy,     100,
			XtNwidth, 360,
			XtNheight,120,
			XtNlabel, "Введите название файла устройства:",
			XtNvalue, "/dev/ttyS0",
			( void* )0);

	XawDialogAddButton( InputDialog, "  Ok  ", OkCallbackProc, ( void* )0 );
	XawDialogAddButton( InputDialog, "Отмена", CancelCallbackProc, ( void* )0 );
	XMoveWindow ( XtDisplay( InputDialog ), XtWindow( InputDialog ), 100, 200 );
	AddTextPoint( TextOut1, "Инициализация" );
}

static int exch (uint8_t* wr_buf, size_t* coun_byte_wr, uint8_t* rd_buf, size_t* coun_byte_rd )
{
	char str[ NAME_MAX + 1  ];
	int count = 2;

	while( 0 != uart.io( wr_buf, coun_byte_wr, rd_buf, coun_byte_rd ))
	{
		AddTextPoint( TextOut1, "Обмен" );
		AddText( TextOut1, STR_ERROR );

		count--;

		if( 0 >= count )
			break;
		else
		{
			AddText( TextOut2, "Время ожидания ответа от модуля М истекло.\nПередача повторного сообщения\n" );
		}
	}

	if( 0 < count )
	{
		sprintf( str, "Передано=%lu Принято=%lu", *coun_byte_wr, *coun_byte_rd );
		AddTextPoint( TextOut2, str );
		AddText( TextOut2, STR_OK );

		sprintf(str, "\nОбмен:");
		AddTextPoint( TextOut1, str);

		if ( (rd_buf[0] == 0x14))
		{
			AddText( TextOut1, STR_OK );
			count = 2;
		}
		else
		{
			AddText( TextOut1, STR_ERROR );
			count = 2;
			sprintf(str, "Ошибка ответного сообщения\n");
			AddText( TextOut2, str);
		}
	}
	return 0;
}

statik void out_buff_msg()
{
	AddText( TextOut2, "Write Data:\n");

	*(( uint16_t* )&out_buf[  2 ]) = data_strus1_1;
	*(( uint16_t* )&out_buf[  4 ]) = data_strus2_1;
	*(( uint16_t* )&out_buf[  6 ]) = data_strus1_2;
	*(( uint16_t* )&out_buf[  8 ]) = data_strus2_2;
	*(( uint16_t* )&out_buf[ 10 ]) = data_strus1_3;
	*(( uint16_t* )&out_buf[ 12 ]) = data_strus2_3;
	*(( uint16_t* )&out_buf[ 14 ]) = data_strus1_4;
	*(( uint16_t* )&out_buf[ 16 ]) = data_strus2_4;
	*(( uint16_t* )&out_buf[ 18 ]) = data_strus1_5;
	*(( uint16_t* )&out_buf[ 20 ]) = data_strus2_5;
	*(( uint16_t* )&out_buf[ 22 ]) = data_strus1_6;
	*(( uint16_t* )&out_buf[ 24 ]) = data_strus2_6;
	*(( uint16_t* )&out_buf[ 26 ]) = data_strus1_7;
	*(( uint16_t* )&out_buf[ 28 ]) = data_strus2_7;
	*(( uint16_t* )&out_buf[ 30 ]) = data_strus1_8;
	*(( uint16_t* )&out_buf[ 32 ]) = data_strus2_8;

	for( i = 0; ( sizeof( out_buf ) -1 ) > i; ++i )
		out_buf[ 34 ] += out_buf[ i ];
}

static void DataCallbackProc (Widget widget, XtPointer closure, XtPointer call_data)
{
	String strus1_1, strus1_2, strus1_3, strus1_4, strus1_5, strus1_6, strus1_7, strus1_8;
	String strus2_1, strus2_2, strus2_3, strus2_4, strus2_5, strus2_6, strus2_7, strus2_8;

	XtVaGetValues( Data1_1, XtNstring, &strus1_1, NULL );
	int i = sscanf( strus1_1, "%x", &data_strus1_1 );
	printf( "%d %d\n", i, data_strus1_1 );

	XtVaGetValues( Data1_2, XtNstring, &strus1_2, NULL );
	i = sscanf( strus1_2, "%x", &data_strus1_2 );
	printf( "%d %d\n", i, data_strus1_2 );

	XtVaGetValues( Data1_3, XtNstring, &strus1_3, NULL );
	i = sscanf( strus1_3, "%x", &data_strus1_3 );
	printf( "%d %d\n", i, data_strus1_3 );

	XtVaGetValues( Data1_4, XtNstring, &strus1_4, NULL );
	i = sscanf( strus1_4, "%x", &data_strus1_4 );
	printf( "%d %d\n", i, data_strus1_4 );

	XtVaGetValues( Data1_5, XtNstring, &strus1_5, NULL );
	i = sscanf( strus1_5, "%x", &data_strus1_5 );
	printf( "%d %d\n", i, data_strus1_5 );

	XtVaGetValues( Data1_6, XtNstring, &strus1_6, NULL );
	i = sscanf( strus1_6, "%x", &data_strus1_6 );
	printf( "%d %d\n", i, data_strus1_6 );

	XtVaGetValues( Data1_7, XtNstring, &strus1_7, NULL );
	i = sscanf( strus1_7, "%x", &data_strus1_7 );
	printf( "%d %d\n", i, data_strus1_7 );

	XtVaGetValues( Data1_8, XtNstring, &strus1_8, NULL );
	i = sscanf( strus1_8, "%x", &data_strus1_8 );
	printf( "%d %d\n", i, data_strus1_8 );

	XtVaGetValues( Data2_1, XtNstring, &strus2_1, NULL );
	i = sscanf( strus2_1, "%x", &data_strus2_1 );
	printf( "%d %d\n", i, data_strus2_1 );

	XtVaGetValues( Data2_2, XtNstring, &strus2_2, NULL );
	i = sscanf( strus2_2, "%x", &data_strus2_2 );
	printf( "%d %d\n", i, data_strus2_2 );

	XtVaGetValues( Data2_3, XtNstring, &strus2_3, NULL );
	i = sscanf( strus2_3, "%x", &data_strus2_3 );
	printf( "%d %d\n", i, data_strus2_3 );

	XtVaGetValues( Data2_4, XtNstring, &strus2_4, NULL );
	i = sscanf( strus2_4, "%x", &data_strus2_4 );
	printf( "%d %d\n", i, data_strus2_4 );

	XtVaGetValues( Data2_5, XtNstring, &strus2_5, NULL );
	i = sscanf( strus2_5, "%x", &data_strus2_5 );
	printf( "%d %d\n", i, data_strus2_5 );

	XtVaGetValues( Data2_6, XtNstring, &strus2_6, NULL );
	i = sscanf( strus2_6, "%x", &data_strus2_6 );
	printf( "%d %d\n", i, data_strus2_6 );

	XtVaGetValues( Data2_7, XtNstring, &strus2_7, NULL );
	i = sscanf( strus2_7, "%x", &data_strus2_7 );
	printf( "%d %d\n", i, data_strus2_7 );

	XtVaGetValues( Data2_8, XtNstring, &strus2_8, NULL );
	i = sscanf( strus2_8, "%x", &data_strus2_8 );
	printf( "%d %d\n", i, data_strus2_8 );

}


static void WrDataUartCallbackProc( Widget widget, XtPointer closure, XtPointer call_data )
{
	int i;

	coun_byte_rd = 3;

	out_buf[0] = 0x41;
	out_buf[1] = 0x01;
	out_buf[34] = 0;

	out_buf_msg();

	printf( "Write: ");
	for( i=0; sizeof( out_buf ) > i; i ++)
		printf( "%d ", out_buf[ i ]);
	printf( "\n" );

	coun_byte_wr = sizeof( out_buf );
	exch( out_buf, &coun_byte_wr, rd_buf, &coun_byte_rd );

	printf( "Read: \n");
	for( i = 0; i < coun_byte_rd; i++ )
		printf( "%02x ", rd_buf[i] );
	printf(" \n");

	char str[ NAME_MAX + 1 ];

	for( i = 0; i < coun_byte_rd; i++ )
	{
		sprintf( str, "%02x ", rd_buf[ i ]);
		AddText( TextOut2, str );
	}
	AddText( TextOut2, "\n");
}

static void PntData ( void )
{
	char str[ NAME_MAX + 1 ];

	sprintf( str, " %02x ", rd_buf[ 3 ]);
	AddText( CCM1, str);
	sprintf( str, "%02x"  , rd_buf[ 2 ]);
	AddText( CCM1, str);
	sprintf( str, " %02x ", rd_buf[ 13 ]);
	AddText( CCM2, str);
	sprintf( str, "%02x"  , rd_buf[ 12 ]);
	AddText( CCM2, str);

	sprintf( str, " %02x ", rd_buf[ 23 ]);
	AddText( CCM3 , str);
	sprintf( str, "%02x"  , rd_buf[ 22 ]);
	AddText( CCM3, str);
	sprintf( str, " %02x ", rd_buf[ 33 ]);
	AddText( CCM4, str);
	sprintf( str, "%02x"  , rd_buf[ 32 ]);
	AddText( CCM4, str);

	sprintf( str, " %02x ", rd_buf[ 5 ]);
	AddText( W1IS11 , str);
	sprintf( str, "%02x"  , rd_buf[ 4 ]);
	AddText( W1IS11, str);
	sprintf( str, " %02x ", rd_buf[ 15 ]);
	AddText( W1IS12, str);
	sprintf( str, "%02x"  , rd_buf[ 14 ]);
	AddText( W1IS12, str);

	sprintf( str, " %02x ", rd_buf[ 25 ]);
	AddText( W1IS13 , str);
	sprintf( str, "%02x"  , rd_buf[ 24 ]);
	AddText( W1IS13, str);
	sprintf( str, " %02x ", rd_buf[ 35 ]);
	AddText( W1IS14, str);
	sprintf( str, "%02x"  , rd_buf[ 34 ]);
	AddText( W1IS14, str);

	sprintf( str, " %02x ", rd_buf[ 7 ]);
	AddText( W1IS21 , str);
	sprintf( str, "%02x"  , rd_buf[ 6 ]);
	AddText( W1IS21, str);
	sprintf( str, " %02x ", rd_buf[ 17 ]);
	AddText( W1IS22, str);
	sprintf( str, "%02x"  , rd_buf[ 16 ]);
	AddText( W1IS22, str);

	sprintf( str, " %02x ", rd_buf[ 27 ]);
	AddText( W1IS23 , str);
	sprintf( str, "%02x"  , rd_buf[ 26 ]);
	AddText( W1IS23, str);
	sprintf( str, " %02x ", rd_buf[ 37 ]);
	AddText( W1IS24, str);
	sprintf( str, "%02x"  , rd_buf[ 36 ]);
	AddText( W1IS24, str);

	sprintf( str, " %02x ", rd_buf[ 9 ]);
	AddText( W2IS11 , str);
	sprintf( str, "%02x"  , rd_buf[ 8 ]);
	AddText( W2IS11, str);
	sprintf( str, " %02x ", rd_buf[ 19 ]);
	AddText( W2IS12, str);
	sprintf( str, "%02x"  , rd_buf[ 18 ]);
	AddText( W2IS12, str);

	sprintf( str, " %02x ", rd_buf[ 29 ]);
	AddText( W2IS13 , str);
	sprintf( str, "%02x"  , rd_buf[ 28 ]);
	AddText( W2IS13, str);
	sprintf( str, " %02x ", rd_buf[ 39 ]);
	AddText( W2IS14, str);
	sprintf( str, "%02x"  , rd_buf[ 38 ]);
	AddText( W2IS14, str);

	sprintf( str, " %02x ", rd_buf[ 11 ]);
	AddText( W2IS21 , str);
	sprintf( str, "%02x"  , rd_buf[ 10 ]);
	AddText( W2IS21, str);
	sprintf( str, " %02x ", rd_buf[ 21 ]);
	AddText( W2IS22, str);
	sprintf( str, "%02x"  , rd_buf[ 20 ]);
	AddText( W2IS22, str);

	sprintf( str, " %02x ", rd_buf[ 31 ]);
	AddText( W2IS23 , str);
	sprintf( str, "%02x"  , rd_buf[ 30 ]);
	AddText( W2IS23, str);
	sprintf( str, " %02x ", rd_buf[ 41 ]);
	AddText( W2IS24, str);
	sprintf( str, "%02x"  , rd_buf[ 40 ]);
	AddText( W2IS24, str);

}

static void RdDataUartCallbackProc (Widget widget, XtPointer closure, XtPointer call_data)
{
	int i;
	coun_byte_rd = 43;
	coun_byte_wr = 3;
	wr_buf[0] = 0x41;
	wr_buf[1] = 0x02;
	wr_buf[2] = 0x43;

	AddText( TextOut2, "Read Data:\n");

	printf( "Write: \n");
	for( i=0; 3 > i; ++i )
		printf( "%02x ", wr_buf[ i ]);

	printf( "\n" );

	exch (wr_buf, &coun_byte_wr, rd_buf, &coun_byte_rd);

	printf( "Read: \n");
	for( i=0; 43 > i; ++i )
		printf( "%02x ", rd_buf[ i ]);

	char str[ NAME_MAX + 1 ];
	for( i = 0; i < 12; i++ )
	{
		sprintf( str, "%02x ", rd_buf[ i ]);
		AddText( TextOut2, str );
	}
	AddText( TextOut2, "\n");

	for( i = 12; i < 24; i++ )
	{
		sprintf( str, "%02x ", rd_buf[ i ]);
		AddText( TextOut2, str );
	}
	AddText( TextOut2, "\n");
	for( i = 24; i < 36; i++ )
	{
		sprintf( str, "%02x ", rd_buf[ i ]);
		AddText( TextOut2, str );
	}
	AddText( TextOut2, "\n");
	for( i = 36; i < 43; i++ )
	{
		sprintf( str, "%02x ", rd_buf[ i ]);
		AddText( TextOut2, str );
	}
	AddText( TextOut2, "\n");
	for( i = 0; i < 43; i++ )
		rd_buf[ i ] = 0;

	PntData();

}

static void CntrLineUartCallbackProc (Widget widget, XtPointer closure, XtPointer call_data)
{
	int i;
	coun_byte_rd = 8;
	coun_byte_wr = 3;
	wr_buf[0] = 0x41;
	wr_buf[1] = 0x03;
	wr_buf[2] = 0x44;

	AddText( TextOut2, "Cntr Line:\n");

	printf( "Write: \n");
	for( i=0; 3 > i; ++i )
		printf( "%02x ", wr_buf[ i ]);
	printf( "\n" );

	exch (wr_buf, &coun_byte_wr, rd_buf, &coun_byte_rd);

	printf( "Read: \n");
	for( i = 0; i < coun_byte_rd; i++ )
		printf( "%02x ", rd_buf[i] );
	printf(" \n");

	char str[ NAME_MAX + 1 ];
	for( i = 0; i < coun_byte_rd; i++ )
	{
		sprintf( str, "%02x ", rd_buf[ i ]);
		AddText( TextOut2, str );
	}
	AddText( TextOut2, "\n");
}

static void StartWrCallbackProc (Widget widget, XtPointer closure, XtPointer call_data)
{
	event.xclient.display = XtDisplay( widget );
	event.xclient.window  = XtWindow ( widget );

	if( True == IsStop )
	{
		IsStop = False,
		XtVaSetValues( CommandWidgetWrCycle, XtNlabel, "Стоп", ( void* )0 ),
		XSendEvent( event.xclient.display, event.xclient.window, False, NoEventMask, &event );
		num_start |= START_WR;
	}
	else
	{
		IsStop = True;
		XtVaSetValues( CommandWidgetWrCycle, XtNlabel, "Старт", ( void* )0 );
		num_start &= ~START_WR;
	}
}

static void StartRdCallbackProc (Widget widget, XtPointer closure, XtPointer call_data)
{
	event.xclient.display = XtDisplay( widget );
	event.xclient.window  = XtWindow ( widget );

	if( True == IsStop )
	{
		IsStop = False,
		XtVaSetValues( CommandWidgetRdCycle, XtNlabel, "Стоп", ( void* )0 ),
		XSendEvent( event.xclient.display, event.xclient.window, False, NoEventMask, &event );
		num_start |= START_RD;
	}
	else
	{
		IsStop = True;
		XtVaSetValues( CommandWidgetRdCycle, XtNlabel, "Старт", ( void* )0 );
		num_start &= ~START_RD;
	}
}

static void StartCntrCallbackProc (Widget widget, XtPointer closure, XtPointer call_data)
{
	event.xclient.display = XtDisplay( widget );
	event.xclient.window  = XtWindow ( widget );

	if( True == IsStop )
	{
		IsStop = False,
		XtVaSetValues( CommandWidgetCntrCycle, XtNlabel, "Стоп", ( void* )0 ),
		XSendEvent( event.xclient.display, event.xclient.window, False, NoEventMask, &event );
		num_start |= START_CNTR;
	}
	else
	{
		IsStop = True;
		XtVaSetValues( CommandWidgetCntrCycle, XtNlabel, "Старт", ( void* )0 );
		num_start &= ~START_CNTR;
	}
}

static void StartCallbackProc (Widget widget, XtPointer closure, XtPointer call_data)
{

	event.xclient.display = XtDisplay( widget );
	event.xclient.window = XtWindow( widget );

	if( True == IsStop )
	{
		IsStop = False,
		XtVaSetValues( CommandWidgetCycle, XtNlabel, "Стоп", ( void* )0 ),
		XSendEvent( event.xclient.display, event.xclient.window, False, NoEventMask, &event );
		num_start |= START_ALL;
	}
	else
	{
		IsStop = True;
		XtVaSetValues( CommandWidgetCycle, XtNlabel, "Старт", ( void* )0 );
		num_start &= ~START_ALL;
	}
}

static int init( void )
{
	int res = 0;

	Widget parent = app_init();

	CommandWidgetInit = XtVaCreateManagedWidget(( _XtString )0, commandWidgetClass, parent, XtNlabel, "Инициализация", XtNwidth, 212, XtNstate, True, ( void* )0 );
	XMoveWindow( XtDisplay( CommandWidgetInit ), XtWindow( CommandWidgetInit ), 20, 10 );
	XtAddCallback( CommandWidgetInit, XtNcallback, InitUartCallbackProc, NULL );


	Widget label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "УС1:              УС2:", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 45, 45 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "1", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 70 );

	Data1_1 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data1_1 ), XtWindow( Data1_1 ), 35, 70 );

	Data2_1 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data2_1 ), XtWindow( Data2_1 ), 130, 70 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "2", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 101 );

	Data1_2 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data1_2 ), XtWindow( Data1_2 ), 35, 101 );

	Data2_2 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data2_2 ), XtWindow( Data2_2 ), 130, 101 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "3", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 132 );

	Data1_3 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data1_3 ), XtWindow( Data1_3 ), 35, 132 );

	Data2_3 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data2_3 ), XtWindow( Data2_3 ), 130, 132 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "4", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 163 );

	Data1_4 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data1_4 ), XtWindow( Data1_4 ), 35, 163 );

	Data2_4 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data2_4 ), XtWindow( Data2_4 ), 130, 163 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "5", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 194 );

	Data1_5 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data1_5 ), XtWindow( Data1_5 ), 35, 194 );

	Data2_5 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data2_5 ), XtWindow( Data2_5 ), 130, 194 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "6", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 225 );

	Data1_6 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data1_6 ), XtWindow( Data1_6 ), 35, 225 );

	Data2_6 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data2_6 ), XtWindow( Data2_6 ), 130, 225 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "7", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 256 );

	Data1_7 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data1_7 ), XtWindow( Data1_7 ), 35, 256 );

	Data2_7 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data2_7 ), XtWindow( Data2_7 ), 130, 256 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "8", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 287 );

	Data1_8 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data1_8 ), XtWindow( Data1_8 ), 35, 287 );

	Data2_8 =  XtVaCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent,
	XtNdisplayCaret, TRUE, XtNeditType, XawtextEdit, XtNwidth, 80, XtNheight, 28, NULL );
	XMoveWindow( XtDisplay( Data2_8 ), XtWindow( Data2_8 ), 130, 287 );

	CommandWidgetData = XtVaCreateManagedWidget(( _XtString )0, commandWidgetClass, parent, XtNlabel, "Ввод значений", XtNwidth, 212, ( void* )0);
	XMoveWindow( XtDisplay( CommandWidgetData ), XtWindow( CommandWidgetData ), 20, 325 );
	XtAddCallback( CommandWidgetData, XtNcallback, DataCallbackProc, ( void* )0 );

	Arg arg_out_tex_in[ 4 ];
	XtSetArg( arg_out_tex_in[ 0 ], XtNscrollVertical, False );
	XtSetArg( arg_out_tex_in[ 1 ], XtNdisplayCaret, False );
	XtSetArg( arg_out_tex_in[ 2 ], XtNwidth, 65 );
	XtSetArg( arg_out_tex_in[ 3 ], XtNheight, 26 );


	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "Данные из М1И-16РУ:", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 30, 383 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "1          2          3          4", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 90, 410 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "ССМ", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 435 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "1 ИС1", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 466 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "1 ИС2", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 497 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "2 ИС1", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 528 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "2 ИС2", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 10, 559 );

	CCM1 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XtVaSetValues( CCM1, XtNstring, ( String )0, ( void* )0 );
	XMoveWindow( XtDisplay( CCM1 ), XtWindow( CCM1 ), 70, 435 );

	CCM2 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XtVaSetValues( CCM2, XtNstring, ( String )0, ( void* )0 );
	XMoveWindow( XtDisplay( CCM2 ), XtWindow( CCM2 ), 132, 435 );

	CCM3 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( CCM3 ), XtWindow( CCM3 ), 194, 435 );

	CCM4 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( CCM4 ), XtWindow( CCM4 ), 256, 435 );

	W1IS11 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W1IS11 ), XtWindow( W1IS11 ), 70, 466 );

	W1IS12 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W1IS12 ), XtWindow( W1IS12 ), 132, 466 );

	W1IS13 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W1IS13 ), XtWindow( W1IS13 ), 194, 466 );

	W1IS14 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W1IS14 ), XtWindow( W1IS14 ), 256, 466 );

	W1IS21 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W1IS21 ), XtWindow( W1IS21 ), 70, 497 );

	W1IS22 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W1IS22 ), XtWindow( W1IS22 ), 132, 497 );

	W1IS23 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W1IS23 ), XtWindow( W1IS23 ), 194, 497 );

	W1IS24 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W1IS24 ), XtWindow( W1IS24 ), 256, 497 );

	W2IS11 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W2IS11 ), XtWindow( W2IS11 ), 70, 528 );

	W2IS12 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W2IS12 ), XtWindow( W2IS12 ), 132, 528 );

	W2IS13 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W2IS13 ), XtWindow( W2IS13 ), 194, 528 );

	W2IS14 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W2IS14 ), XtWindow( W2IS14 ), 256, 528 );

	W2IS21 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W2IS11 ), XtWindow( W2IS21 ), 70, 559 );

	W2IS22 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W2IS12 ), XtWindow( W2IS22 ), 132, 559 );

	W2IS23 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W2IS13 ), XtWindow( W2IS23 ), 194, 559 );

	W2IS24 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex_in, sizeof( arg_out_tex_in ) /sizeof( arg_out_tex_in[ 0 ]));
	XMoveWindow( XtDisplay( W2IS14 ), XtWindow( W2IS24 ), 256, 559 );

	Widget box = XtVaCreateManagedWidget(( _XtString )0, formWidgetClass, parent, XtNwidth, 300, XtNheight, 180, XtNborderWidth, 1, ( void* )0 );
	XMoveWindow( XtDisplay( box ), XtWindow( box ), 20, 610 );

	label = XtVaCreateManagedWidget(( _XtString )0, labelWidgetClass, box, XtNlabel, "Передача команды", XtNwidth, 200, XtNborderWidth, 0, ( void* )0 );
	XMoveWindow( XtDisplay( label ), XtWindow( label ), 45, 5 );

	label = XtVaCreateManagedWidget(( _XtString )0, labelWidgetClass, box, XtNlabel, "Однократно              Циклически", XtNwidth, 270, XtNborderWidth, 0, ( void* )0 );
	XMoveWindow( XtDisplay( label ), XtWindow( label ), 15, 30 );

	CommandWidgetWrData = XtVaCreateManagedWidget(( _XtString )0, commandWidgetClass, parent, XtNlabel, "Write Data", XtNwidth, 130, ( void* )0);
	XMoveWindow( XtDisplay( CommandWidgetWrData ), XtWindow( CommandWidgetWrData ), 30, 670 );
	XtAddCallback( CommandWidgetWrData, XtNcallback, WrDataUartCallbackProc, ( void* )0 );

	CommandWidgetWrCycle = XtVaCreateManagedWidget(( _XtString )0, commandWidgetClass, parent, XtNlabel, "Старт", XtNwidth, 130, ( void* )0 );
	XMoveWindow( XtDisplay( CommandWidgetWrCycle ), XtWindow( CommandWidgetWrCycle ), 180, 670 );
	XtAddCallback( CommandWidgetWrCycle, XtNcallback, StartWrCallbackProc, ( void* )0 );

	CommandWidgetRdData = XtVaCreateManagedWidget(( _XtString )0, commandWidgetClass, parent, XtNlabel, "Read Data", XtNwidth, 130, ( void* )0);
	XMoveWindow( XtDisplay( CommandWidgetRdData ), XtWindow( CommandWidgetRdData ), 30, 710 );
	XtAddCallback( CommandWidgetRdData, XtNcallback, RdDataUartCallbackProc, ( void* )0 );

	CommandWidgetRdCycle = XtVaCreateManagedWidget(( _XtString )0, commandWidgetClass, parent, XtNlabel, "Старт", XtNwidth, 130, ( void* )0 );
	XMoveWindow( XtDisplay( CommandWidgetRdCycle ), XtWindow( CommandWidgetRdCycle ), 180, 710 );
	XtAddCallback( CommandWidgetRdCycle, XtNcallback, StartRdCallbackProc, ( void* )0 );

	CommandWidgetCntrLine = XtVaCreateManagedWidget(( _XtString )0, commandWidgetClass, parent, XtNlabel, "Cntr Line", XtNwidth, 130, ( void* )0);
	XMoveWindow( XtDisplay( CommandWidgetCntrLine ), XtWindow( CommandWidgetCntrLine ), 30, 750 );
	XtAddCallback( CommandWidgetCntrLine, XtNcallback, CntrLineUartCallbackProc, ( void* )0 );

	CommandWidgetCntrCycle = XtVaCreateManagedWidget(( _XtString )0, commandWidgetClass, parent, XtNlabel, "Старт", XtNwidth, 130, ( void* )0 );
	XMoveWindow( XtDisplay( CommandWidgetCntrCycle ), XtWindow( CommandWidgetCntrCycle ), 180, 750 );
	XtAddCallback( CommandWidgetCntrCycle, XtNcallback, StartCntrCallbackProc, ( void* )0 );

	box = XtVaCreateManagedWidget(( _XtString )0, formWidgetClass, parent, XtNwidth, 212, XtNheight, 70, XtNborderWidth, 1, ( void* )0 );
	XMoveWindow( XtDisplay( box ), XtWindow( box ), 20, 805 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "Циклическая проверка", XtNwidth, 200, XtNborderWidth, 0, ( void* )0 );
	XMoveWindow( XtDisplay( label ), XtWindow( label ), 25, 810 );

	CommandWidgetCycle = XtVaCreateManagedWidget(( _XtString )0, commandWidgetClass, parent, XtNlabel, "Старт", XtNwidth, 190, ( void* )0 );
	XMoveWindow( XtDisplay( CommandWidgetCycle ), XtWindow( CommandWidgetCycle ), 30, 835 );
	XtAddCallback( CommandWidgetCycle, XtNcallback, StartCallbackProc, ( void* )0 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "Результат проверки", XtNborderWidth, 0, ( void* )0);
	XMoveWindow( XtDisplay( label), XtWindow( label ), 330, 5 );

	Arg arg_out_tex[ 4 ];
	XtSetArg( arg_out_tex[ 0 ], XtNscrollVertical, XawtextScrollAlways );
	XtSetArg( arg_out_tex[ 1 ], XtNdisplayCaret, False );
	XtSetArg( arg_out_tex[ 2 ], XtNwidth, 630  );
	XtSetArg( arg_out_tex[ 3 ], XtNheight, 300 );

	TextOut1 = XtCreateManagedWidget( ( _XtString )0, asciiTextWidgetClass, parent,  arg_out_tex, sizeof( arg_out_tex ) /sizeof( arg_out_tex[ 0 ]));
	XMoveWindow( XtDisplay( TextOut1 ), XtWindow( TextOut1 ), 330, 30 );

	label = XtVaCreateManagedWidget( ( _XtString )0, labelWidgetClass, parent, XtNlabel, "Дополнительная информация", XtNborderWidth, 0, ( void* )0 );
	XMoveWindow( XtDisplay( label ), XtWindow( label ), 330, 340 );

	TextOut2 = XtCreateManagedWidget(( _XtString )0, asciiTextWidgetClass, parent, arg_out_tex, sizeof( arg_out_tex ) /sizeof( arg_out_tex[ 0 ]));
	XMoveWindow( XtDisplay( TextOut2 ), XtWindow( TextOut2 ), 330, 370 );

	Widget exit = XtVaCreateManagedWidget(( _XtString )0, commandWidgetClass, parent, XtNlabel, "Выход", XtNwidth, 212, ( void* )0 );
	XMoveWindow( XtDisplay( exit ), XtWindow( exit ), 20, 890 );
	XtAddCallback( exit, XtNcallback, ExitCallbackProc, ( void* )0 );

	return( res );
}

static Boolean AppWorkProc( XtPointer closure )
{
	event.xclient.display = XtDisplay( closure );
	event.xclient.window = XtWindow( closure );
	event.xclient.data.s[ 0 ] = BEG_TEST;
	XSendEvent( event.xclient.display, event.xclient.window, False, NoEventMask, &event );
	XFlush(XtDisplay( closure ));
	XtVaSetValues( TextOut2, XtNstring, ( String )0, ( void* )0 );

	if( num_start & START_WR )
	{
		int i;
		struct timespec ts = { tv_sec: 0, tv_nsec: 5000000 };
		nanosleep( &ts, ( struct timespec* )0 );

		coun_byte_rd = 3;

		out_buf[0] = 0x41;
		out_buf[1] = 0x01;
		out_buf[34] = 0;

		out_buf_msg();

		printf( "Write: ");
		for( i=0; sizeof( out_buf ) > i; i ++)
			printf( "%d ", out_buf[ i ]);
		printf( "\n" );

		coun_byte_wr = sizeof( out_buf );
		exch( out_buf, &coun_byte_wr, rd_buf, &coun_byte_rd );

		printf( "Read: \n");
		for( i = 0; i < coun_byte_rd; i++ )
			printf( "%02x ", rd_buf[i] );
		printf(" \n");

		char str[ NAME_MAX + 1 ];

		for( i = 0; i < coun_byte_rd; i++ )
		{
			sprintf( str, "%02x ", rd_buf[ i ]);
			AddText( TextOut2, str );
		}
		AddText( TextOut2, "\n");

		nanosleep( &ts, ( struct timespec* )0 );
		event.xclient.data.s[ 0 ] = END_TEST;
		XSendEvent( event.xclient.display, event.xclient.window, False, NoEventMask, &event );
	}

	if( num_start & START_RD )
	{
		int i;
		char str[ NAME_MAX + 1 ];
		struct timespec ts = { tv_sec: 0, tv_nsec: 5000000 };
		nanosleep( &ts, ( struct timespec* )0 );

		coun_byte_rd = 43;
		coun_byte_wr = 3;
		wr_buf[0] = 0x41;
		wr_buf[1] = 0x02;
		wr_buf[2] = 0x43;

		AddText( TextOut2, "Read Data:\n");

		for( i = 0; i < 43; i++)
			rd_buf[i] = 0;

		uart.io( wr_buf, &coun_byte_wr, rd_buf, &coun_byte_rd );

		PntData();

		for( i = 0; i < 12; i++ )
		{
			sprintf( str, "%02x ", rd_buf[ i ]);
			AddText( TextOut2, str );
		}
		AddText( TextOut2, "\n");

		for( i = 12; i < 24; i++ )
		{
			sprintf( str, "%02x ", rd_buf[ i ]);
			AddText( TextOut2, str );
		}
		AddText( TextOut2, "\n");
		for( i = 24; i < 36; i++ )
		{
			sprintf( str, "%02x ", rd_buf[ i ]);
			AddText( TextOut2, str );
		}
		AddText( TextOut2, "\n");
		for( i = 36; i < 43; i++ )
		{
			sprintf( str, "%02x ", rd_buf[ i ]);
			AddText( TextOut2, str );
		}

		if ( rd_buf[0] != 0x14)
		{
			sprintf(str, "\nОшибка ответного сообщения");
			AddText( TextOut2, str);
		}
		AddText( TextOut2, "\n");

		nanosleep( &ts, ( struct timespec* )0 );
		event.xclient.data.s[ 0 ] = END_TEST;
		XSendEvent( event.xclient.display, event.xclient.window, False, NoEventMask, &event );
	}

	if( num_start & START_CNTR )
	{
		int i;
		struct timespec ts = { tv_sec: 0, tv_nsec: 5000000 };
		nanosleep( &ts, ( struct timespec* )0 );

		coun_byte_rd = 8;
		coun_byte_wr = 3;
		wr_buf[0] = 0x41;
		wr_buf[1] = 0x03;
		wr_buf[2] = 0x44;

		AddText( TextOut2, "Cntr Line:\n");

		printf( "Write: \n");
		for( i=0; 3 > i; ++i )
			printf( "%02x ", wr_buf[ i ]);
		printf( "\n" );

		exch (wr_buf, &coun_byte_wr, rd_buf, &coun_byte_rd);

		printf( "Read: \n");
		for( i = 0; i < coun_byte_rd; i++ )
			printf( "%02x ", rd_buf[i] );
		printf(" \n");

		char str[ NAME_MAX + 1 ];
		for( i = 0; i < coun_byte_rd; i++ )
		{
			sprintf( str, "%02x ", rd_buf[ i ]);
			AddText( TextOut2, str );
		}
		AddText( TextOut2, "\n");

		nanosleep( &ts, ( struct timespec* )0 );

		event.xclient.data.s[ 0 ] = END_TEST;
		XSendEvent( event.xclient.display, event.xclient.window, False, NoEventMask, &event );
	}

	if( num_start & START_ALL )
	{
		int i;

		coun_byte_rd = 3;

		out_buf[0] = 0x41;
		out_buf[1] = 0x01;
		out_buf[34] = 0;

		out_buf_msg();

		printf( "Write: ");
		for( i=0; sizeof( out_buf ) > i; i ++)
			printf( "%02x ", out_buf[ i ]);
		printf( "\n" );

		coun_byte_wr = sizeof( out_buf );
		exch( out_buf, &coun_byte_wr, rd_buf, &coun_byte_rd );

		char str[ NAME_MAX + 1 ];

		for( i = 0; i < coun_byte_rd; i++ )
		{
			sprintf( str, "%02x ", rd_buf[ i ]);
			AddText( TextOut2, str );
		}

		if(	( ( 0x14 == rd_buf[ 0 ]) & ( 0x01 == out_buf[ 1 ]) & ( 0x01 == ( rd_buf[ 1 ] & 0x0F )) ) ||
			( ( 0x14 == rd_buf[ 0 ]) & ( 0x02 == out_buf[ 1 ]) & ( 0x02 == ( rd_buf[ 1 ] & 0x0F )) ) ||
			( ( 0x14 == rd_buf[ 0 ]) & ( 0x03 == out_buf[ 1 ]) & ( 0x03 == ( rd_buf[ 1 ] & 0x0F )) ) )
		{
			AddText( TextOut2, "\n");
		}
		else
		{
			sprintf(str, "\nОшибка ответного сообщения");
			AddText( TextOut2, str);
			AddText( TextOut2, "\n");
		}

		struct timespec ts = { tv_sec: 0, tv_nsec: 5000000 };
		nanosleep( &ts, ( struct timespec* )0 );

		coun_byte_rd = 43;
		coun_byte_wr = 3;
		wr_buf[0] = 0x41;
		wr_buf[1] = 0x02;
		wr_buf[2] = 0x43;

		AddText( TextOut2, "Read Data:\n");

		for( i = 0; i < 43; i++)
			rd_buf[i] = 0;

		uart.io( wr_buf, &coun_byte_wr, rd_buf, &coun_byte_rd );

		PntData();

		for( i = 0; i < 12; i++ )
		{
				sprintf( str, "%02x ", rd_buf[ i ]);
				AddText( TextOut2, str );
		}
		AddText( TextOut2, "\n");

		for( i = 12; i < 24; i++ )
		{
			sprintf( str, "%02x ", rd_buf[ i ]);
			AddText( TextOut2, str );
		}
		AddText( TextOut2, "\n");
		for( i = 24; i < 36; i++ )
		{
			sprintf( str, "%02x ", rd_buf[ i ]);
			AddText( TextOut2, str );
		}
		AddText( TextOut2, "\n");
		for( i = 36; i < 43; i++ )
		{
			sprintf( str, "%02x ", rd_buf[ i ]);
			AddText( TextOut2, str );
		}

		printf( "rd_buf: %02x ", rd_buf[0]);
		if ( rd_buf[0] != 0x14)
		{
			sprintf(str, "\nОшибка ответного сообщения");
			AddText( TextOut2, str);
		}
		AddText( TextOut2, "\n");

		nanosleep( &ts, ( struct timespec* )0 );

		event.xclient.data.s[ 0 ] = END_TEST;
		XSendEvent( event.xclient.display, event.xclient.window, False, NoEventMask, &event );

	}
	return( True );
}

#define STR_START_CYCLE_TEST	"Начало проверки   : "
#define STR_CYCLE_TEST_OK		"Циклов без ошибок : "
#define STR_CYCLE_TEST_ERROR	"Циклов c ошибками : "
#define STR_CYCLE_TEST_ALL		"Циклов всего      : "
#define STR_STOP_CYCLE_TEST		"Окончание проверки: "

static int run ( void )
{
	char str[ NAME_MAX + 1 ];
	XEvent event;
	struct timespec start_cycle_test_time;

	while( FALSE == XtAppGetExitFlag( AppContext ))
	{
		XtAppNextEvent( AppContext, &event );

		if( ClientMessage == event.type )
		{
			switch( event.xclient.data.s[ 0 ])
			{
				case START:
					XtVaSetValues( TextOut1, XtNstring, ( String )0, ( void* )0 );
					XtVaSetValues( TextOut2, XtNstring, ( String )0, ( void* )0 );

					clock_gettime( CLOCK_REALTIME, &start_cycle_test_time );
					AddTextPoint( TextOut1, "Проверка модуля М" );

					XtAppAddWorkProc( AppContext, AppWorkProc, Desktop );
					printf( "START\n" );
					break;

				case BEG_TEST:
					snprintf( str, NAME_MAX, "%s %s\n",
							STR_START_CYCLE_TEST, ctime( &start_cycle_test_time.tv_sec ));
					XtVaSetValues( TextOut1, XtNstring, str, ( void* )0 );
					printf( "BEG_TEST\n" );
					break;

				case END_TEST:
					snprintf( str, NAME_MAX, "%s %s\n",
							STR_START_CYCLE_TEST, ctime( &start_cycle_test_time.tv_sec ));
					XtVaSetValues( TextOut1, XtNstring, ( String )0, ( void* )0 );

					if( False == IsStop )
					{
						XtAppAddWorkProc( AppContext, AppWorkProc, Desktop );
					}
					else
					{
						time_t t = time(( time_t* )0 );
						XtVaSetValues( CommandWidgetCycle, XtNsensitive, True, ( void* )0 );
						XtVaSetValues( CommandWidgetInit, XtNsensitive, True, ( void* )0 );
						strcat( strcat( strcat( str, STR_STOP_CYCLE_TEST ), ctime( &t )), "\n" );
					}

					XtVaSetValues( TextOut1, XtNstring, str, ( void* )0 );
					break;

				default:;
			}
		}
		XtDispatchEvent( &event );
	}

	uart.deinit();
	return( 0 );
}

static int deinit( void )
{
	XtAppSetExitFlag( AppContext );
	uart.deinit();
	return 0;
}

struct _app_t app =
{
	init: init,
	run: run,
	deinit: deinit
};

