
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <system.h>
#include <uart.h>

static void port_init( void );
static void install_dummy_isr( void );
static void show_sys_info( void );

void isr_SWI_dummy(void)__attribute__((interrupt ("SWI")));
void isr_UNDEF_dummy(void)__attribute__((interrupt ("UNDEF")));
void isr_IRQ_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_FIQ_dummy(void)__attribute__((interrupt ("FIQ")));
void isr_PABORT_dummy(void)__attribute__((interrupt ("ABORT")));
void isr_DABORT_dummy(void)__attribute__((interrupt ("ABORT")));
void isr_ADC_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_RTC_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_UTXD1_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_UTXD0_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_SIO_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_IIC_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_URXD1_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_URXD0_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_TIMER5_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_TIMER4_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_TIMER3_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_TIMER2_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_TIMER1_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_TIMER0_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_UERR01_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_WDT_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_BDMA1_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_BDMA0_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_ZDMA1_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_ZDMA0_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_TICK_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_PB_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_ETHERNET_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_TS_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_KEYPAD_dummy(void)__attribute__((interrupt ("IRQ")));
void isr_USB_dummy(void)__attribute__((interrupt ("IRQ")));

void sys_init( void )
{
    WTCON = 0;	//Watchdog deshabilitado
    INTMSK = ~0; //Enmascara todas las interrupciones

    LOCKTIME = 0xFFF; //Estabilizaci?n del PLL: 512 us
    PLLCON = 0x38021; //Frecuencia del  MCLK_SLOW: 500 KHz
    CLKSLOW = 0x8; //Frecuencia del MCLK: 64 MHz
    CLKCON = 0x7FF8; //Modo de funcionamiento normal y Reloj distribuido a todos lo controladores

    SBUSCON = 0x8000001B; //Prioridades de bus del sistema fijas: LCD > ZDMA > BDMA > IRQ (por defecto)

    SYSCFG = 0x0; //Cache deshabilitada
    I_PMST = 0x1F1B; // mGA > mGB > mGC > mGD y EINT0 > EINT1 > EINT2 > EINT3
    I_PSLV = 0x1B1B1B1B; //URxD0 > URxD1 > IIC > SIO y TIMER0> TIMER1 > TIMER2 > TIMER3 y ZDMA0 > ZDMA1 > BDMA0 > BDMA1
    INTMOD = 0x0; // IRQ habilitado y FIQ deshabilitado
    install_dummy_isr();//Instala RTI por defecto a todas los tipos de interrupci?n
    EXTINTPND = ~(0);//Borra todas las interrupciones pendientes
    I_ISPC = ~(0);//Borra interrupciones externas pendientes por  la l?nea EINT[7:4]
    INTCON = 0x1;

    SET_OPMODE(SVCMODE);//Pone el procesador en modo SVC
    SET_IRQFLAG(0);//Habilita en el procesador las interrupciones IRQ
    SET_FIQFLAG(1);//Deshabilita en el procesador las interrupciones FIQ

    port_init();
    uart0_init();

    show_sys_info();

}

static void port_init( void )
{
	//Inicializa los 22 registros mapeados en memoria del controlador interno de puertos de E/S
    PDATA = ~0;
    PCONA = 0xFE;

    PDATB = ~0;
    PCONB = 0x14F;

    PDATC = ~0;
    PCONC = 0x5FF555FF;
    PUPC = 0x94FB;

    PDATD = ~0;
    PCOND = 0xAAAA;
    PUPD = 0xFF;

    PDATE = ~0;
    PCONE = 0x255A9;
    PUPE = 0x1FB;

    PDATF = ~0;
    PCONF = 0x251A;
    PUPF = 0x74;

    PDATG = ~0;
    PCONG = 0xF5FF;
    PUPG = 0x30;

    SPUCR = 0x7;

    EXTINT = 0x22000220;

}

static void install_dummy_isr( void )
{ //Instala cada RTI en su lugar de la tabla virtual de vectores de IRQ (v?ase s3cev40.h)

    pISR_SWI = (uint32)isr_SWI_dummy;
    pISR_UNDEF = (uint32)isr_UNDEF_dummy;
    pISR_IRQ = (uint32)isr_IRQ_dummy;
    pISR_FIQ = (uint32)isr_FIQ_dummy;
    pISR_PABORT = (uint32)isr_PABORT_dummy;
    pISR_DABORT = (uint32)isr_DABORT_dummy;
    pISR_ADC = (uint32)isr_ADC_dummy;
    pISR_RTC = (uint32)isr_RTC_dummy;
    pISR_UTXD1 = (uint32)isr_UTXD1_dummy;
    pISR_UTXD0 = (uint32)isr_UTXD0_dummy;
    pISR_SIO = (uint32)isr_SIO_dummy;
    pISR_IIC = (uint32)isr_IIC_dummy;
    pISR_URXD1 = (uint32)isr_URXD1_dummy;
    pISR_URXD0 = (uint32)isr_URXD0_dummy;
    pISR_TIMER5 = (uint32)isr_TIMER5_dummy;
    pISR_TIMER4 = (uint32)isr_TIMER4_dummy;
    pISR_TIMER3 = (uint32)isr_TIMER3_dummy;
    pISR_TIMER2 = (uint32)isr_TIMER2_dummy;
    pISR_TIMER1 = (uint32)isr_TIMER1_dummy;
    pISR_TIMER0 = (uint32)isr_TIMER0_dummy;
    pISR_UERR01 = (uint32)isr_UERR01_dummy;
    pISR_WDT = (uint32)isr_WDT_dummy;
    pISR_BDMA1 = (uint32)isr_BDMA1_dummy;
    pISR_BDMA0 = (uint32)isr_BDMA0_dummy;
    pISR_ZDMA1 = (uint32)isr_ZDMA1_dummy;
    pISR_ZDMA0 = (uint32)isr_ZDMA0_dummy;
    pISR_TICK = (uint32)isr_TICK_dummy;
    pISR_PB = (uint32)isr_PB_dummy;
    pISR_ETHERNET = (uint32)isr_ETHERNET_dummy;
    pISR_TS = (uint32)isr_TS_dummy;
    pISR_KEYPAD = (uint32)isr_KEYPAD_dummy;
    pISR_USB = (uint32)isr_USB_dummy;

}        

void isr_SWI_dummy( void ) //Cada RTI muestra alg?n mensaje de error y bloquea el sistema
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_SWI_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_UNDEF_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_UNDEF_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_IRQ_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_IRQ_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_FIQ_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_FIQ_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_PABORT_dummy( void )
{
	uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_PABORT_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_DABORT_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_DABORT_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_ADC_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_ADC_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_RTC_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_RTC_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_UTXD1_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_UTXD1_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_UTXD0_dummy( void )
{
	uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_UTXD0_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_SIO_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_SIO_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_IIC_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_IIC_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_URXD1_dummy( void )
{
	uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_URXD1_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_URXD0_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_URXD0_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_TIMER5_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_TIMER5_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_TIMER4_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_TIMER4_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_TIMER3_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_TIMER3_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_TIMER2_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_TIMER2_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_TIMER1_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_TIMER1_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_TIMER0_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_TIMER0_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_UERR01_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_UERR01_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_WDT_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_WDT_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_BDMA1_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_BDMA1_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_BDMA0_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_BDMA0_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_ZDMA1_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_ZDMA1_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_ZDMA0_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_ZDMA0_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_TICK_dummy( void )
{
    uart0_puts( "\n\n*** ERROR FATAL: ejecutando isr_TICK_dummy" );
    SEGS = 0x75;
    while( 1 );
}

void isr_PB_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_PB_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_ETHERNET_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_ETHERNET_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_TS_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_TS_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_KEYPAD_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_KEYPAD_dummy");
    SEGS = 0x75;
    while (1);
}

void isr_USB_dummy( void )
{
    uart0_puts("\n\n*** ERROR FATAL: ejecutando isr_USB_dummy");
    SEGS = 0x75;
    while (1);
}

static void show_sys_info( void )
{
    uart0_puts("\n\n****Mensaje inicial****");
}

inline void sleep( void )
{
    CLKCON |= (1 << 2);    // Pone a la CPU en estado IDLE
}

