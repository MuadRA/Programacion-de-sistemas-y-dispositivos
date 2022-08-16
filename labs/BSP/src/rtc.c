
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <rtc.h>

extern void isr_TICK_dummy( void );

void rtc_init( void )
{
	TICNT = 0x00;
    RTCALM = 0x0;
    RTCRST = 0x0;


    RTCCON = 0x1;
    //Inicializa la hora/fecha a las 00:00:00 del martes 1 de enero de 2013
    BCDYEAR = 0x13;
    BCDMON = 0x1;
    BCDDAY = 0x1;
    BCDDATE = 0x3;
    BCDHOUR = 0x0;
    BCDMIN = 0x0;
    BCDSEC = 0x0;

    ALMYEAR = 0x0;
    ALMMON = 0x0;
    ALMDAY = 0x0;
    ALMHOUR = 0x0;
    ALMMIN = 0x0;
    ALMSEC = 0x0;


    RTCCON &= ~(1 << 0);
}

void rtc_puttime( rtc_time_t *rtc_time )
{
    RTCCON |= (1 << 0); //habilita el acceso a los registros de hora/fecha del RTC
    //binario a BCD
    BCDYEAR = ((((rtc_time->year)/10)<<4) + (rtc_time->year)%10);
    BCDMON  = ((((rtc_time->mon)/10)<<4) + (rtc_time->mon)%10);
    BCDDAY  = ((((rtc_time->mday)/10)<<4) + (rtc_time->mday)%10);
    BCDDATE = ((((rtc_time->wday)/10)<<4) + (rtc_time->wday)%10);
    BCDHOUR = ((((rtc_time->hour)/10)<<4) + (rtc_time->hour)%10);
    BCDMIN  = ((((rtc_time->min)/10)<<4) + (rtc_time->min)%10);
    BCDSEC  = ((((rtc_time->sec)/10)<<4) + (rtc_time->sec)%10);
        
    RTCCON &= ~(1 << 0); //deshabilita el acceso a los registros de hora/fecha del RTC
}

void rtc_gettime( rtc_time_t *rtc_time )
{
	RTCCON |= (1 << 0);
    //BCD a binario
    rtc_time->year = (((BCDYEAR)&15) + ((BCDYEAR)>>4)*10);
    rtc_time->mon  = (((BCDMON)&15) + ((BCDMON)>>4)*10);
    rtc_time->mday = (((BCDDAY)&15) + ((BCDDAY)>>4)*10);
    rtc_time->wday = (((BCDDATE)&15) + ((BCDDATE)>>4)*10);
    rtc_time->hour = (((BCDHOUR)&15) + ((BCDHOUR)>>4)*10);
    rtc_time->min  = (((BCDMIN)&15) + ((BCDMIN)>>4)*10);
    rtc_time->sec  = (((BCDSEC)&15) + ((BCDSEC)>>4)*10);
    if( ! rtc_time->sec ){
        rtc_time->year = (((BCDYEAR)&15) + ((BCDYEAR)>>4)*10);
        rtc_time->mon  = (((BCDMON)&15) + ((BCDMON)>>4)*10);
        rtc_time->mday = (((BCDDAY)&15) + ((BCDDAY)>>4)*10);
        rtc_time->wday = (((BCDDATE)&15) + ((BCDDATE)>>4)*10);
        rtc_time->hour = (((BCDHOUR)&15) + ((BCDHOUR)>>4)*10);
        rtc_time->min  = (((BCDMIN)&15) + ((BCDMIN)>>4)*10);
        rtc_time->sec  = (((BCDSEC)&15) + ((BCDSEC)>>4)*10);
    };

    RTCCON &= ~(1 << 0);
}


void rtc_open( void (*isr)(void), uint8 tick_count )
{
	pISR_TICK = (uint32) isr; //instala la ISR argumento en la tabla virtual de vectores de IRQ
	I_ISPC = (1<<20); //borra flag de interrupción pendiente por ticks de RTC
	INTMSK &= ~((1<<26) | (1<<20)); //desenmascara globalmente interrupciones e interrupciones por tick de RTC
	TICNT = (1<<7) | tick_count; //habilita en el RTC la generación de ticks y fija el valor del contador que los genera
}

void rtc_close( void )
{
    TICNT   &= ~(1<<7); //deshabilita en el RTC la generación de ticks
    INTMSK   |= (1 << 26) | (1 << 20); //enmascara interrupciones por tick de RTC
    pISR_TICK = (uint32)isr_TICK_dummy; //instala isr_TICK_dummy en la tabla virtual de vectores de interrupción
}
