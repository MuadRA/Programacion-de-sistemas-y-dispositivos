
#include <s3c44b0x.h>
#include <lcd.h>

extern uint8 font[];
static uint8 lcd_buffer[LCD_BUFFER_SIZE];

//static uint8 state;

void lcd_init( void )
{      
    DITHMODE = 0x12210;
    DP1_2    = 0xA5A5;
    DP4_7    = 0xBA5DA65;
    DP3_5    = 0xA5A5F;
    DP2_3    = 0xD6B;
    DP5_7    = 0xEB7B5ED;
    DP3_4    = 0x7DBE;
    DP4_5    = 0x7EBDF ;
    DP6_7    = 0x7FDFBFE;
    
    REDLUT   = 0x0;
    GREENLUT = 0x0;
    BLUELUT  = 0x0;

    LCDCON1  = 0x1C020;
    LCDCON2  = 0x13CEF;
    LCDCON3  = 0x0;

    LCDSADDR1 = (2 << 27) | ((uint32)lcd_buffer >> 1);
    LCDSADDR2 = (1 << 29) | (((uint32)lcd_buffer + LCD_BUFFER_SIZE) & 0x3FFFFF) >> 1;
    LCDSADDR3 = 0x50;
    
    lcd_off();
}

void lcd_on( void )
{
    LCDCON1 |= (1 << 0);
}

void lcd_off( void )
{
	LCDCON1 &= ~(1 << 0);
}

uint8 lcd_status( void )
{
    if(LCDCON1 & (1 << 0)){
    	return ON;
    }
    else{
    	return OFF;
    }
}

void lcd_clear( void )
{
	//fila 1: (0,0) (1,0) ... (319,0)etc.
	//fila 239(ultima): (0,239) (1,239) ... (319,239)
	int i,j;
    for(i = 0; i < LCD_HEIGHT;i++){
    	for(j = 0; j < LCD_WIDTH; j++){
    		lcd_putpixel(j,i,WHITE); // relleno en blanco todo
    	}
    }
}

void lcd_putpixel( uint16 x, uint16 y, uint8 c)
{
    uint8 byte, bit;
    uint16 i;

    i = x/2 + y*(LCD_WIDTH/2);
    bit = (1-x%2)*4;
    
    byte = lcd_buffer[i];
    byte &= ~(0xF << bit);
    byte |= c << bit;
    lcd_buffer[i] = byte;
}

uint8 lcd_getpixel( uint16 x, uint16 y )
{
    uint8 byte, bit, ret;
    uint16 i;

    i = x/2 + y*(LCD_WIDTH/2);
    bit = (1-x%2)*4;

    byte = lcd_buffer[i];

    if(bit == 0){
    	byte &= ~(0xF << 4);
    	ret = byte;
    }
    else{
    	byte &= ~(0xF << 0);
    	ret = byte >> bit;
    }

    return ret;
}

void lcd_draw_hline( uint16 xleft, uint16 xright, uint16 y, uint8 color, uint16 width )
{
	int i,j;
	for(i = y; i < (y+width); i++ ){
		for(j = xleft; j < xright; j++){
			lcd_putpixel(j,i,color);
		}
	}
}

void lcd_draw_vline( uint16 yup, uint16 ydown, uint16 x, uint8 color, uint16 width )
{
	int i,j;
	for (i = yup; i < ydown; i++) {
		for (j = x; j < (x + width); j++) {
			lcd_putpixel(j, i, color);
		}
	}
}

void lcd_draw_box( uint16 xleft, uint16 yup, uint16 xright, uint16 ydown, uint8 color, uint16 width )
{
	lcd_draw_hline(xleft, xright, yup, color, width);
	lcd_draw_vline(yup, ydown, xleft, color, width);
	lcd_draw_hline(xleft, xright, ydown, color, width);
	lcd_draw_vline(yup, ydown + width + 1, xright, color, width);
}

void lcd_putchar( uint16 x, uint16 y, uint8 color, char ch )
{
    uint8 line, row;
    uint8 *bitmap;

    bitmap = font + ch*16; //posición de comienzo del mapa de bits del caracter
    for( line=0; line<16; line++ ) //Recorre el mapa de bits, línea a línea (de arriba abajo)
    								//y columna a columna (de izquierda a derecha)
        for( row=0; row<8; row++ )                    
            if( bitmap[line] & (0x80 >> row) )
                lcd_putpixel( x+row, y+line, color );
            else
                lcd_putpixel( x+row, y+line, WHITE );
}

void lcd_puts( uint16 x, uint16 y, uint8 color, char *s )
{
	uint16 i = y, j;

	while((*s != '\0')&&(i < LCD_HEIGHT)){ // mientras no llegue al fin de linea
		j = x;
		while((*s != '\0')&&(j < LCD_WIDTH)){
			lcd_putchar(j,i,color,*s); // introduzco la letra
			s++;
			j+=8;
		}
		i+=16;
	}
}

void lcd_putint( uint16 x, uint16 y, uint8 color, int32 i )
{
	uint16 x2 = x, y2 = y;
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

	lcd_puts(x2,y2,color,p);
}

void lcd_puthex( uint16 x, uint16 y, uint8 color, uint32 i )
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

    lcd_puts(x,y,color,p);
}

void lcd_putchar_x2( uint16 x, uint16 y, uint8 color, char ch )
{
	uint8 line, row;
	uint8* bitmap;
	bitmap = font + ch * 16;
	for (line = 0; line < 32; line += 2)
		for (row = 0; row < 16; row += 2)
			if (bitmap[(line / 2)] & (0x80 >> (row / 2))) {
				lcd_putpixel(x + row, y + line, color);
				lcd_putpixel(x + row, (y + line + 1), color);
				lcd_putpixel((x + row + 1), y + line, color);
				lcd_putpixel((x + row + 1), (y + line + 1), color);
			}
			else {
				lcd_putpixel(x + row, y + line, WHITE);
				lcd_putpixel(x + row, (y + line + 1), WHITE);
				lcd_putpixel((x + row + 1), y + line, WHITE);
				lcd_putpixel((x + row + 1), (y + line + 1), WHITE);
			}
}

void lcd_puts_x2( uint16 x, uint16 y, uint8 color, char *s )
{
	uint16 i = y, j;

	while((*s != '\0')&&(i < LCD_HEIGHT)){
		j = x;
		while((*s != '\0')&&(j < LCD_WIDTH)){
			lcd_putchar_x2(j,i,color,*s);
			s++;
			j+=16;
		}
		i+=32;
	}
}

void lcd_putint_x2( uint16 x, uint16 y, uint8 color, int32 i )
{
	uint16 x2 = x, y2 = y;
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

	lcd_puts_x2(x2,y2,color,p);
}

void lcd_puthex_x2( uint16 x, uint16 y, uint8 color, uint32 i )
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

    lcd_puts_x2(x,y,color,p);
}

void lcd_putWallpaper( uint8 *bmp )
{
    uint32 headerSize;

    uint16 x, ySrc, yDst;
    uint16 offsetSrc, offsetDst;

    headerSize = bmp[10] + (bmp[11] << 8) + (bmp[12] << 16) + (bmp[13] << 24);

    bmp = bmp + headerSize;
    
    for( ySrc=0, yDst=LCD_HEIGHT-1; ySrc<LCD_HEIGHT; ySrc++, yDst-- )                                                                       
    {
        offsetDst = yDst*LCD_WIDTH/2;
        offsetSrc = ySrc*LCD_WIDTH/2;
        for( x=0; x<LCD_WIDTH/2; x++ )
            lcd_buffer[offsetDst+x] = ~bmp[offsetSrc+x];
    }
}
