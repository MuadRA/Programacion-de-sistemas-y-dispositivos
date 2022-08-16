
#include <s3c44b0x.h>
#include <uart.h>

void uart0_init( void )
{
	UFCON0 =  0x1;
	UMCON0 =  0x0;
	ULCON0 =  0x3;
	UBRDIV0 = 0x22;
	UCON0 =  0x5;
}

void uart0_putchar( char ch )
{
    while( UFSTAT0 & (1<<9) ); // mientras este lleno rx no puedo poner el char
    UTXH0 = ch;
}        

char uart0_getchar( void )
{
	while(!(UFSTAT0 & 15)); // mientras no introduzca algo el usuario me quedo en el bucle
	return URXH0;
}

void uart0_puts( char *s )
{
	while(*s != '\0'){ // no llego al fin de línea
		uart0_putchar(*s); // meto el caracter
		s++; // aumento el puntero
	}
}

void uart0_putint( int32 i )
{
	char buf[8 + 1];
	char *p = buf + 8;
	uint8 c = 0;
	uint8 neg = 0;

	if(i < 0){
		neg = 1;
		i = -i;
	}

	*p = '\0';
	do {
		c = i % 10; 
		*--p = '0' + c; 
		i = i / 10; 
	} while (i);

	if(neg == 1){
		*--p = '-';
	}

	uart0_puts(p);
}

void uart0_puthex( uint32 i )
{
    char buf[8 + 1];
    char *p = buf + 8;
    uint8 c;

    *p = '\0';

    do {
        c = i & 0xf;
        if( c < 10 )
            *--p = '0' + c;
        else
            *--p = 'a' + c - 10;
        i = i >> 4;
    } while( i );

    uart0_puts( p );
}

void uart0_gets( char *s )
{
	char caracter = uart0_getchar();

	while(caracter != '\n'){
		*s = caracter;
		s++;
		caracter = uart0_getchar();
	}
	*s = '\0';
}

int32 uart0_getint( void )
{
	int32 dev = 0;
	int8 negativo = 1;
	char n[128], *p;
	uart0_gets(n);
	p = n;
	if(*p == '-'){
		negativo = 0;
		p++;
	}
	while(*p != '\0'){
		dev = dev + *p - '0';
		if(*p != '\0'){
			dev *= 10;
		}
		++p;
	}
	if(negativo == 0){
		dev *= -1;
	}
	return dev;
}

uint32 uart0_gethex( void )
{
	char buf[128];
	char *p = buf;
	uint32 c;
	uint32 aux = 0;

	uart0_gets(p);
	do {
		if (*p <= '9')
			c = *p - '0';
		else if((*p == 'A' || *p == 'B' || *p == 'C' || *p == 'D' || *p == 'E' || *p == 'F'))
			c = *p - 'A' + 10;
		else
			c = *p - 'a' + 10;

		aux += c;
		++p;
		if (*p != '\0') {
			aux = aux << 4;
		}
	} while (*p != '\0');
	return aux;
}
