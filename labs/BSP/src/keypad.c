
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <keypad.h>

extern void isr_KEYPAD_dummy( void );

uint8 keypad_scan( void )
{
    uint8 aux;

    aux = *( KEYPAD_ADDR + 0x1c );//máscara de scan: 0b00011100

    if( (aux & 0x0f) != 0x0f )//comprueba si la tecla pulsada está en la fila 1
    {
        if( (aux & 0x8) == 0 )//comprueba si está en la columna 1
            return KEYPAD_KEY0;
        else if( (aux & 0x4) == 0 )//comprueba si está en la columna 2
            return KEYPAD_KEY1;
        else if( (aux & 0x2) == 0 )
            return KEYPAD_KEY2;
        else if( (aux & 0x1) == 0 )
            return KEYPAD_KEY3;
    }

    aux = *( KEYPAD_ADDR + 0x1a);
    if( (aux & 0x0f) != 0x0f )
    {
    	if( (aux & 0x8) == 0 )
    		return KEYPAD_KEY4;
    	else if( (aux & 0x4) == 0 )
    		return KEYPAD_KEY5;
    	else if( (aux & 0x2) == 0 )
    		return KEYPAD_KEY6;
    	else if( (aux & 0x1) == 0 )
    		return KEYPAD_KEY7;
    }

    aux = *( KEYPAD_ADDR + 0x16);
    if( (aux & 0x0f) != 0x0f )
    {
    	if( (aux & 0x8) == 0 )
    		return KEYPAD_KEY8;
    	else if( (aux & 0x4) == 0 )
    		return KEYPAD_KEY9;
    	else if( (aux & 0x2) == 0 )
    		return KEYPAD_KEYA;
    	else if( (aux & 0x1) == 0 )
    		return KEYPAD_KEYB;
    }

    aux = *( KEYPAD_ADDR + 0x0e);
    if( (aux & 0x0f) != 0x0f )
    {
    	if( (aux & 0x8) == 0 )
    	    return KEYPAD_KEYC;
    	else if( (aux & 0x4) == 0 )
    	    return KEYPAD_KEYD;
    	else if( (aux & 0x2) == 0 )
    	    return KEYPAD_KEYE;
    	else if( (aux & 0x1) == 0 )
    	    return KEYPAD_KEYF;
    }

    return KEYPAD_FAILURE;
}

uint8 keypad_status( uint8 scancode )
{
	if (keypad_scan() == scancode){
	    return KEY_DOWN;
	}
	else{
	    return KEY_UP;
	}
}

void keypad_open( void (*isr)(void) )
{
	pISR_KEYPAD	= (uint32) isr;
	I_ISPC = BIT_KEYPAD;
	INTMSK	&= ~(BIT_GLOBAL | BIT_KEYPAD);
}

void keypad_close( void )
{
	INTMSK |=  BIT_KEYPAD;
	pISR_KEYPAD	= (uint32) isr_KEYPAD_dummy;
}

#if KEYPAD_IO_METHOD == POOLING


void keypad_init( void )
{
	EXTINT = (EXTINT & ~(0xf<<4)) | (2<<4);
};


void keypad_wait_keydown( uint8 scancode )
{
     while( 1 ) 
     {
        while( PDATG & (1 << 1) ); // espera presión del keypad
        sw_delay_ms( KEYPAD_KEYDOWN_DELAY ); // eliminación de rebotes
        if ( scancode == keypad_scan() )// si se pulsa la tecla indicada por scancode
            return;
        while( (PDATG &(1 << 1)) == 0 ); // si no lo es, espera depresión y vulve a empezar
        sw_delay_ms( KEYPAD_KEYUP_DELAY );
    }         
}

void keypad_wait_keyup( uint8 scancode )
{
	 while( 1 )
	 {
	      while( PDATG & (1 << 1) );
	      sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	      if ( scancode == keypad_scan() )
	      {
		      while( (PDATG &(1 << 1)) == 0 );
		      sw_delay_ms( KEYPAD_KEYUP_DELAY );
	          return;
	      }
	  }
}

void keypad_wait_any_keydown( void )
{
	while(PDATG & (1<<1) );
	sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	while( (PDATG & ( 1 << 1)) == 0 );
    sw_delay_ms( KEYPAD_KEYUP_DELAY );
}

void keypad_wait_any_keyup( void )
{
	while(PDATG & (1<<1));
	sw_delay_ms( KEYPAD_KEYUP_DELAY );
	while( (PDATG & (1<<1)) == 0 );
}

uint8 keypad_getchar( void )
{
	uint8 scancode;
    while(PDATG & (1<<1)); // esperar presión
    sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
    scancode = keypad_scan();// guardo el scancode de la key pulsada

    while( (PDATG & (1<<1)) == 0 ); // espero depresión
    sw_delay_ms( KEYPAD_KEYUP_DELAY );

    return scancode; // lo devuelvo
}

uint8 keypad_getchartime( uint16 *ms )
{
	uint8 scancode;

	while(PDATG & (1<<1));// espera presión
	timer3_start();
	sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	scancode = keypad_scan();// scancode key pulsada

	while( (PDATG & (1<<1)) == 0 );// espera depresion
	*ms = timer3_stop() / 10; // detengo timer y calculo los ms
	sw_delay_ms( KEYPAD_KEYUP_DELAY );

	return scancode;
}

uint8 keypad_timeout_getchar( uint16 ms )
{
	uint8 scancode;

	timer3_start_timeout(ms); // hacemos un timeout con los ms pasados por parámetro
	while(timer3_timeout() && (PDATG & (1<<1)));
	if(!timer3_timeout( ))
	    	return KEYPAD_TIMEOUT;
	else{
		sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
		scancode = keypad_scan();

		while( timer3_timeout() && ((PDATG & (1<<1)) == 0) );
		sw_delay_ms( KEYPAD_KEYUP_DELAY );

		return scancode;
	}
}

#elif KEYPAD_IO_METHOD == INTERRUPT

static uint8 key = KEYPAD_FAILURE;

static void keypad_down_isr( void ) __attribute__ ((interrupt ("IRQ")));
static void timer0_down_isr( void ) __attribute__ ((interrupt ("IRQ")));
static void keypad_up_isr( void ) __attribute__ ((interrupt ("IRQ")));
static void timer0_up_isr( void ) __attribute__ ((interrupt ("IRQ")));

void keypad_init( void )
{
    EXTINT = (EXTINT & ~(0xf<<4)) | (2<<4);	// Falling edge tiggered
    timers_init();
    keypad_open( keypad_down_isr );
};

uint8 keypad_getchar( void )
{
	uint8 scancode;

    while( key == KEYPAD_FAILURE );
    scancode = key;
    key = KEYPAD_FAILURE;
    return scancode;
}

static void keypad_down_isr( void )
{
	timer0_open_ms( timer0_down_isr, KEYPAD_KEYDOWN_DELAY, TIMER_ONE_SHOT );
	INTMSK   |= BIT_KEYPAD;				   // Enmascara la interrupciÃ³n por pulsaciÃ³n del keypad
	I_ISPC	  = BIT_KEYPAD;				   // Borra el bit de interrupciÃ³n pendiente
}

static void timer0_down_isr( void )
{
	if( !fifo_is_full( &fifo ) )
		fifo_enqueue( &fifo, keypad_scan() );
	EXTINT = (EXTINT & ~(0xf<<4)) | (4<<4);	// Rising edge triggered
	keypad_open( keypad_up_isr );
	I_ISPC = BIT_TIMER0;				   // Borra el bit de interrupciÃ³n pendiente
}

static void keypad_up_isr( void )
{
	timer0_open_ms( timer0_up_isr, KEYPAD_KEYUP_DELAY, TIMER_ONE_SHOT );
	INTMSK   |= BIT_KEYPAD;				   // Enmascara la interrupciÃ³n por pulsaciÃ³n del keypad
	I_ISPC	  = BIT_KEYPAD;				   // Borra el bit de interrupciÃ³n pendiente
}

static void timer0_up_isr( void )
{
	EXTINT = (EXTINT & ~(0xf<<4)) | (2<<4);	// Falling edge tiggered
	keypad_open( keypad_down_isr );
	I_ISPC = BIT_TIMER0;				   // Borra el bit de interrupciÃ³n pendiente
}

#else
	#error No se ha definido el metodo de E/S del keypad
#endif
