#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <adc.h>
#include <lcd.h>
#include <ts.h>

#define PX_ERROR    (5)

static uint16 Vxmin = 0;
static uint16 Vxmax = 0;
static uint16 Vymin = 0;
static uint16 Vymax = 0;

static uint8 state;

extern void isr_TS_dummy( void );

static void ts_scan( uint16 *Vx, uint16 *Vy );
static void ts_calibrate( void );
static void ts_sample2coord( uint16 Vx, uint16 Vy, uint16 *x, uint16 *y );

void ts_init( void )
{
    lcd_init();
    adc_init();
    PDATE |= ((1<<5) | (1<<4) | (1<<7));
    PDATE &= ~(1<<6);
    ts_on();
    ts_calibrate();
    ts_off();
}

void ts_on( void )
{
	adc_on();
	state = TS_ON;
}

void ts_off( void )
{
	adc_off();
	state = TS_OFF;
}

uint8 ts_status( void )
{
	return state;
}

static void ts_calibrate( void )
{
    uint16 x, y;
    uint16 Vx, Vy;
    

    lcd_on();

    do {
    	lcd_clear();
    	lcd_putpixel(0,0,BLACK);
    	lcd_putpixel(0,1,BLACK);
    	lcd_putpixel(0,3,BLACK);
    	lcd_putpixel(1,0,BLACK);
    	lcd_putpixel(1,1,BLACK);
    	lcd_putpixel(1,2,BLACK);
    	lcd_putpixel(1,3,BLACK);

        while((PDATG  & (1 << 2))); // espera presion touchpad
        sw_delay_ms( TS_DOWN_DELAY );
        ts_scan( &Vxmin, &Vymax );
        while(!(PDATG & (1 << 2))); // espera depresion touchpad
        sw_delay_ms( TS_UP_DELAY );

        lcd_putpixel(319,239, BLACK);
        lcd_putpixel(318,239, BLACK);
        lcd_putpixel(317,239, BLACK);
        lcd_putpixel(319,238, BLACK);
        lcd_putpixel(319,237, BLACK);
        lcd_putpixel(318,238, BLACK);

        while((PDATG  & (1 << 2)));
        sw_delay_ms( TS_DOWN_DELAY );
        ts_scan( &Vxmax, &Vymin );
        while(!(PDATG  & (1 << 2)));
        sw_delay_ms( TS_UP_DELAY );

        lcd_putpixel(160,120, BLACK);
        lcd_putpixel(161,120,BLACK);
        lcd_putpixel(159,120, BLACK);
        lcd_putpixel(159,119, BLACK);
        lcd_putpixel(161,119, BLACK);
        lcd_putpixel(160,119, BLACK);

        while((PDATG  & (1 << 2)));
        sw_delay_ms( TS_DOWN_DELAY );
        ts_scan( &Vx, &Vy );
        while(!(PDATG  & (1 << 2)));
        sw_delay_ms( TS_UP_DELAY );
        ts_sample2coord( Vx, Vy, &x, &y );

    
    } while(
    		(x > 160+PX_ERROR) || (x < 160-PX_ERROR)
    		|| (y > 120+PX_ERROR) || (y < 120-PX_ERROR)
    		);

    
}

void ts_wait_down( void )
{
	 while((PDATG  & (1 << 2)));
	 sw_delay_ms( TS_DOWN_DELAY );
}

void ts_wait_up( void )
{
	while((PDATG & (1 << 2)));
    sw_delay_ms( TS_DOWN_DELAY );
    while(!(PDATG & (1 << 2)));
    sw_delay_ms( TS_UP_DELAY );
}

void ts_getpos( uint16 *x, uint16 *y )
{
	uint16 Vx = 0;
	uint16 Vy = 0;

	while((PDATG  & (1 << 2)));
	sw_delay_ms( TS_DOWN_DELAY );
	ts_scan( &Vx, &Vy );
	while(!(PDATG & (1 << 2)));
	sw_delay_ms( TS_UP_DELAY );

	ts_sample2coord( Vx, Vy, x, y );
}

void ts_getpostime( uint16 *x, uint16 *y, uint16 *ms )
{
	uint16 Vx = 0;
	uint16 Vy = 0;

	timer3_start();
	while((PDATG & (1 << 2)));
	timer3_start();
	sw_delay_ms( TS_DOWN_DELAY );
	ts_scan( &Vx, &Vy );

	while(!(PDATG & (1 << 2)));
	*ms = timer3_stop() / 10;
	sw_delay_ms( TS_UP_DELAY );

	ts_sample2coord( Vx, Vy, x, y );
}

uint8 ts_timeout_getpos( uint16 *x, uint16 *y, uint16 ms )
{
	uint16 Vx = 0;
	uint16 Vy = 0;
	
	timer3_start_timeout(ms);
	while(timer3_timeout() && (PDATG & (1 << 2)));
	if(!timer3_timeout( ))
			 return TS_TIMEOUT;
	else{
		sw_delay_ms( TS_DOWN_DELAY );
		ts_scan( &Vx, &Vy );

		while(!(PDATG & (1 << 2)));
		sw_delay_ms( TS_UP_DELAY );
		ts_sample2coord( Vx, Vy, x, y );
		
		return TS_OK;
	}
}

static void ts_scan( uint16 *Vx, uint16 *Vy )
{
    
    PDATE &= ~((1<<7) | (1<<4));
    PDATE |= ((1<<6)| (1<<5));
    *Vx = adc_getSample(ADC_AIN1);
    sw_delay_ms( 1 );

    PDATE |= ((1<<7) | (1<<4));
    PDATE &= ~((1<<6)| (1<<5));
    *Vy = adc_getSample(ADC_AIN0);
    sw_delay_ms( 1 );

    PDATE |= ((1<<5) | (1<<4) | (1<<7));
    PDATE &= ~(1<<6);
    sw_delay_ms( 1 );
}

static void ts_sample2coord( uint16 Vx, uint16 Vy, uint16 *x, uint16 *y )
{
    if( Vx < Vxmin )
        *x = 0;
    else if( Vx > Vxmax )
        *x = 319;
    else 
        *x = (320*(Vx-Vxmin)) / (Vxmax-Vxmin);
    if( Vy < Vymin )
        *y = 239;
    else if( Vy > Vymax )
        *y = 0;
    else
    *y = (239) - (240*(Vy-Vymin)/(Vymax-Vymin));
}

void ts_open( void (*isr)(void) )
{
	pISR_TS = (uint32) isr;
	I_ISPC = BIT_EINT4567;
	INTMSK &= ~(BIT_GLOBAL | BIT_EINT4567);
}

void ts_close( void )
{
	INTMSK   |= (BIT_GLOBAL | BIT_EINT4567);
	pISR_TS = (uint32)isr_TS_dummy;
}
