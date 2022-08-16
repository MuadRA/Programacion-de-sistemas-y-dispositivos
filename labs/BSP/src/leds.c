
#include <s3c44b0x.h>
#include <leds.h>

void leds_init( void )
{
	PCONB &= ~( (1<<10) | (1<<9) );
	led_off(LEFT_LED);
	led_off(RIGHT_LED);
}

void led_on( uint8 led )
{
	if(led == LEFT_LED){
		PDATB &= ~(1 << 9); //pongo un 0 en la 9 posicion
	}
	else{
		PDATB &= ~(1 << 10); // pongo un 0 en la d�cima posici�n
	}

}

void led_off( uint8 led )
{
	if(led == LEFT_LED){
		PDATB |= (1 << 9); // pongo un 1 en la 9 posici�n
	}

	else{
		PDATB |= (1 << 10); // pongo un 1 en la 10 posici�n
	}
}

void led_toggle( uint8 led )
{
	if(led == LEFT_LED){
		PDATB ^= (1 << 9); //invierto
	}

	else{
		PDATB ^= (1<<10); //invierto
	}
}

uint8 led_status( uint8 led )//a & (1 << 4)si bit 4 vale uno se ejecuta
{
	if((led == LEFT_LED) && !(PDATB & (1 << 9))){// si es el led izquierdo y est� en ON (est� a 0)
		return ON;
	}
	else if((led == LEFT_LED) && (PDATB & (1 << 9))){
		return OFF;
	}
	else if((led == RIGHT_LED) && !(PDATB & (1 << 10))){
		return ON;
	}
	else{
		return OFF;
	}
}
