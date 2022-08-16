
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <pbs.h>
#include <timers.h>

extern void isr_PB_dummy( void );

void pbs_init( void )
{
    timers_init();
}

uint8 pb_scan( void )
{
    if( pb_status(PB_LEFT) )
    	return PB_LEFT;
    else if( pb_status(PB_RIGHT) )
    	return PB_RIGHT;
    else
        return PB_FAILURE;
}

uint8 pb_status( uint8 scancode )
{
	if((scancode & PDATG) == 0) // devuelo 1 si está pulsado
		return 1;
	else // devuelvo 0 si no está pulsado
		return 0;
}

void pb_wait_keydown( uint8 scancode )
{
	while((PDATG & scancode) != 0); // espero a que se presione el boton indicado por el scancode(Left o Right)
	sw_delay_ms( PB_KEYDOWN_DELAY ); // espera SW(el timer 3 está ocupado) fin de rebotes
}

void pb_wait_keyup( uint8 scancode )
{
	while((PDATG & scancode) != 0);// espero a que se presione el boton indicado por el scancode(Left o Right)
	sw_delay_ms( PB_KEYDOWN_DELAY );// espera SW(el timer 3 está ocupado) fin de rebotes
	while((PDATG & scancode) == 0);// espero a que se depresione el boton indicado por el scancode(Left o Right)
	sw_delay_ms( PB_KEYUP_DELAY );// espera SW(el timer 3 está ocupado) fin de rebotes
}

void pb_wait_any_keydown( void )
{
	while(((PDATG & 0x40) == 0x40) && ((PDATG & 0x80) == 0x80));// espero a que se pulse cualquier boton
	sw_delay_ms( PB_KEYDOWN_DELAY );// espera SW(el timer 3 está ocupado) fin de rebotes
}

void pb_wait_any_keyup( void )
{
	while(((PDATG & 0x40) == 0x40) && ((PDATG & 0x80) == 0x80)); // espero cualquier presión
	sw_delay_ms( PB_KEYDOWN_DELAY ); // eliminación de rebotes
	while(((PDATG & 0x40) == 0x00) && ((PDATG & 0x80) == 0x00)); // espero cualquier depresión
	sw_delay_ms( PB_KEYUP_DELAY ); // eliminación de rebotes
}

uint8 pb_getchar( void )
{
	uint8 scancode;

	pb_wait_any_keydown();
	scancode = pb_scan();

	while((scancode & PDATG) == 0); // mientras esté pulsado
	sw_delay_ms( PB_KEYUP_DELAY );

	return scancode;
}

uint8 pb_getchartime( uint16 *ms )
{
    uint8 scancode;
    
    while( ((PDATG & 0x40) == 0x40) && ((PDATG & 0x80) == 0x80) );// esperar presión de cualquier pulsador
    timer3_start();// arranca el timer 3 (0,1ms de resolución)
    sw_delay_ms( PB_KEYDOWN_DELAY ); // espera SW(el timer 3 está ocupado) fin de rebotes
    
    scancode = pb_scan(); // obtiene el código del pulsador presionado
    
    while( (scancode & PDATG) == 0 );// espera depresión del pulsador
    *ms = timer3_stop() / 10; // detiene el timer 3 y calcula los ms
    sw_delay_ms( PB_KEYUP_DELAY );// espera SW (el timer 3 está ocupado) fin de rebotes

    return scancode;//devuelve el código del pulsador presionado
}

uint8 pb_timeout_getchar( uint16 ms )
{
	uint8 scancode;
	timer3_start_timeout(ms);
	while(timer3_timeout() && ((PDATG & 0x40) == 0x40) && ((PDATG & 0x80) == 0x80));
	if(!timer3_timeout()){
	    return PB_TIMEOUT;
	}
	else{
		sw_delay_ms( PB_KEYDOWN_DELAY );
	    scancode = pb_scan();
	    while((scancode & PDATG) == 0);
	    sw_delay_ms( PB_KEYUP_DELAY );
	    return scancode;
	}
}

void pbs_open( void (*isr)(void) )
{
    pISR_PB   = (uint32) isr;
    EXTINTPND = 0xf;
    I_ISPC    = BIT_EINT4567;
    INTMSK   &= ~(BIT_GLOBAL | BIT_EINT4567);
}

void pbs_close( void )
{
	INTMSK  |= BIT_EINT4567;
	pISR_PB  =(uint32) isr_PB_dummy;
}
