
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>

extern void isr_TIMER0_dummy( void );

static uint32 loop_ms = 0;
static uint32 loop_s = 0;

static void sw_delay_init( void );

void timers_init( void )
{
	TCFG0 =  0x00000000;
	TCFG1 =  0x00000000;

	TCNTB0 = 0x0000;
	TCNTB2 = 0x0000;
	TCNTB3 = 0x0000;
	TCNTB4 = 0x0000;
	TCNTB5 = 0x0000;
	TCMPB0 = 0x0000;
	TCMPB1 = 0x0000;
	TCMPB2 = 0x0000;
	TCMPB3 = 0x0000;
	TCMPB4 = 0x0000;

    TCON = ((1<<25) | (1<<21) | (1<<17) | (1<<13) | (1<<9) | (1<<1));
    TCON = (TCON & ~(0xfffffff));

    sw_delay_init();
}

static void sw_delay_init( void )
{
    uint32 i;
    
    timer3_start();
    for( i=1000000; i; i--);
    loop_s = ((uint64)1000000*10000)/timer3_stop();
    loop_ms = loop_s / 1000;
};

void timer3_delay_ms( uint16 n )
{
    for( ; n; n-- )
    {
    	TCFG0 = (TCFG0 & ~(0xff << 8)) | (0 << 8);
    	TCFG1 = (TCFG1 & ~(0xf << 12)) | (0 << 12);
    	TCNTB3 = 32000;
    	TCON = (TCON & ~(0xf << 16)) | (1 << 17);
    	TCON = (TCON & ~(0xf << 16)) | (1 << 16);
    	while( !TCNTO3 );
    	while( TCNTO3 );
    }
}

void sw_delay_ms( uint16 n )
{
    uint32 i;
    
    for( i=loop_ms*n; i; i-- );
}

void timer3_delay_s( uint16 n )
{
	//(N+1)∙D = (63+1)∙32
	//1s -> N = 63 D = 32 (63+1)∙32/(64 MHz)= 32microsegundos  (1 s)/(32micros)= 31250
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (63 << 8);//T2‐T3 prescaler: N=63
    TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12);//T3 divisor (1/32): D=32
    TCNTB3 = 31250;//T3 count: C = 31250
    TCON = (TCON & ~(0xf << 16)) | (1 << 17);//one shot, carga TCNT3, stop T3
    TCON = (TCON & ~(0xf << 16)) | (1 << 16);//one shot, no carga TCNT3, start T3
    while( !TCNTO3 );//espera a que TCNTO3 se actualice
    while( TCNTO3 );//espera a que TCNTO3 sea 0
}

void sw_delay_s( uint16 n )
{
    uint32 i;
    
    for( i=loop_s*n; i; i-- );
}

void timer3_start( void ) 
{
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (199 << 8);//T2‐T3 prescaler: N=199
    TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12);//T3 divisor (1/32): D=32
    
    TCNTB3 = 0xffff; //T3 count:C = máximo
    TCON = (TCON & ~(0xf << 16)) | (1 << 17);//one shot, carga TCNT3, stop T3
    TCON = (TCON & ~(0xf << 16)) | (1 << 16);//one shot, no carga TCNT3, start T3
    while( !TCNTO3 );//espera a que TCNTO3 se actualice
}

uint16 timer3_stop( void )
{
    TCON &= ~(1 << 16); // Detiene el timer
    return 0xffff - TCNTO3;// calcula los ciclos de cuenta transcurridos
}

void timer3_start_timeout( uint16 n ) 
{
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (199 << 8);//T2‐T3 prescaler: N=199
    TCFG1 = (TCFG1 & ~(0xf << 12)) | (4 << 12);//T3 divisor (1/32): D=32
    
    TCNTB3 = n; //T3 count: C = timeout
    TCON = (TCON & ~(0xf << 16)) | (1 << 17);//one shot, carga TCNT3, stop T3
    TCON = (TCON & ~(0xf << 16)) | (1 << 16);//one shot, no carga TCNT3, start T3
    while( !TCNTO3 );//espera a que TCNTO3 se actualice
}

uint16 timer3_timeout( )
{
    return !TCNTO3;
}    

void timer0_open_tick( void (*isr)(void), uint16 tps )
{
    pISR_TIMER0 = (uint32)isr;
    I_ISPC      = BIT_TIMER0;
    INTMSK     &= ~(BIT_GLOBAL | BIT_TIMER0);

    if( tps > 0 && tps <= 10 ) { //programa el T0 con resolución de 25micros (40 KHz)
    	TCFG0  = (TCFG0 & ~(0xff)) | (49);
        TCFG1  = (TCFG1 & ~(0xf)) | (4);
        TCNTB0 = (40000U / tps);
    } else if( tps > 10 && tps <= 100 ) { //programa el T0 con resolución de 2,5micros (400 KHz)
    	TCFG0  = (TCFG0 & ~(0xff)) | (19);
    	TCFG1  = (TCFG1 & ~(0xf)) | (2);
        TCNTB0 = (400000U / (uint32) tps);
    } else if( tps > 100 && tps <= 1000 ) { //programa el T0 con resolución de 0,25micros (4 MHz)
    	TCFG0  = (TCFG0 & ~(0xff)) | (7);
        TCFG1  = (TCFG1 & ~(0xf)) | (0);
        TCNTB0 = (4000000U / (uint32) tps);
    } else if ( tps > 1000 ) { //programa el T0 con resolución de 31,25 ns (32 MHz)
    	TCFG0  = (TCFG0 & ~(0xff));
    	TCFG1  = (TCFG1 & ~(0xf));
        TCNTB0 = (32000000U / (uint32) tps);
    }

    TCON = (TCON & ~(0xf << 0)) | (1 << 3)| (1 << 1);//interval, carga TCNT0, stop T0
    TCON = (TCON & ~(0xf << 0)) | (1 << 3) | (1 << 0);//interval, no carga TCNT0, start T0
}

void timer0_open_ms( void (*isr)(void), uint16 ms, uint8 mode )
{
	pISR_TIMER0 = (uint32)isr;
	I_ISPC      = BIT_TIMER0;
	INTMSK     &= ~(BIT_GLOBAL | BIT_TIMER0);

	TCFG0 = (TCFG0 & ~(0xff)) | (199);
	TCFG1 = (TCFG1 & ~(0xf)) | (4);
    TCNTB0 = 10*ms;

	TCON = (TCON & ~(0xf << 0)) | (mode << 3)| (1 << 1);
	TCON = (TCON & ~(0xf << 0)) | (mode << 3) | (1 << 0);
}

void timer0_close( void )
{
	TCNTB0 = 0x0000;
	TCMPB0 = 0x0000;

    TCON = (TCON & ~(0xf << 0)) | (1 << 1);
	TCON = (TCON & ~(0xf << 0));

	INTMSK     |= BIT_TIMER0;
    pISR_TIMER0 = (uint32)isr_TIMER0_dummy;
}

