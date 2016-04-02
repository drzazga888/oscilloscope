#include "LPC17xx.h"                    // Device header
#include "AsciiLib.h"
#include "lcd.h"

uint16_t (*lcd_pattern)(uint16_t, uint16_t) = 0;

static void wait_delay(int count){
	while(count--);
}

static void delay_ms(uint16_t ms){
	uint16_t i, j; 
	for( i = 0; i < ms; i++ )
		for( j = 0; j < 1141; j++ );
}

static __attribute__((always_inline)) void LCD_Send(uint16_t byte){
	LPC_GPIO2->FIODIR |= 0xFF;          /* P2.0...P2.7 Output */
	LCD_DIR(1)		   				            /* Interface A->B */
	LCD_EN(0)	                          /* Enable 2A->2B */
	LPC_GPIO2->FIOPIN =  byte;          /* Write D0..D7 */
	LCD_LE(1)                         
	LCD_LE(0)							              /* latch D0..D7	*/
	LPC_GPIO2->FIOPIN =  byte >> 8;     /* Write D8..D15 */
}

static __attribute__((always_inline)) void LCD_WriteIndex(uint16_t index){
	LCD_CS(0);
	LCD_RS(0);
	LCD_RD(1);
	LCD_Send(index); 
	wait_delay(25);	
	LCD_WR(0);  
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

static __attribute__((always_inline)) void LCD_WriteData(uint16_t data){				
	LCD_CS(0);
	LCD_RS(1);   
	LCD_Send(data);
	LCD_WR(0);     
	wait_delay(1);
	LCD_WR(1);
	LCD_CS(1);
}

static __attribute__((always_inline)) void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue){   
	LCD_WriteIndex(LCD_Reg);             /* Write 16-bit Index, then Write Reg */
	LCD_WriteData(LCD_RegValue);         /* Write 16-bit Reg */
}

static void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos){
	#if  (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
 	uint16_t temp = Xpos;
	Xpos = Ypos;
	Ypos = (MAX_X - 1) - temp;  
	#endif
	LCD_WriteReg(0x0020, Xpos);
	LCD_WriteReg(0x0021, Ypos);
}

void lcd_init(void){
	LPC_GPIO0->FIODIR |= 0x03f80000;
	LPC_GPIO0->FIOSET = 0x03f80000;
	delay_ms(100);
	LCD_WriteReg(0x00e7,0x0010);      
	LCD_WriteReg(0x0000,0x0001);  	/* start internal osc */
	LCD_WriteReg(0x0001,0x0100);     
	LCD_WriteReg(0x0002,0x0700); 	/* power on sequence */
	#if  (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
	LCD_WriteReg(0x0003,(1<<12)|(0<<5)|(1<<4)|(0<<3) ); 	/* importance */
	#elif  (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)
	LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3) ); 	/* importance */
	#endif
	LCD_WriteReg(0x0004,0x0000);                                   
	LCD_WriteReg(0x0008,0x0207);	           
	LCD_WriteReg(0x0009,0x0000);         
	LCD_WriteReg(0x000a,0x0000); 	/* display setting */        
	LCD_WriteReg(0x000c,0x0001);	/* display setting */        
	LCD_WriteReg(0x000d,0x0000); 			        
	LCD_WriteReg(0x000f,0x0000);
	/* Power On sequence */
	LCD_WriteReg(0x0010,0x0000);   
	LCD_WriteReg(0x0011,0x0007);
	//LCD_WriteReg(0x0011,0x000F);
	LCD_WriteReg(0x0012,0x0000);                                                                 
	LCD_WriteReg(0x0013,0x0000);                 
	delay_ms(50);  /* delay 50 ms */		
	LCD_WriteReg(0x0010,0x1590);   
	LCD_WriteReg(0x0011,0x0227);
	delay_ms(50);  /* delay 50 ms */		
	LCD_WriteReg(0x0012,0x009c);                  
	delay_ms(50);  /* delay 50 ms */		
	LCD_WriteReg(0x0013,0x1900);   
	LCD_WriteReg(0x0029,0x0023);
	LCD_WriteReg(0x002b,0x000e);
	delay_ms(50);  /* delay 50 ms */		
	LCD_WriteReg(0x0020,0x0000);                                                            
	LCD_WriteReg(0x0021,0x0000);           
	delay_ms(50);  /* delay 50 ms */		
	LCD_WriteReg(0x0030,0x0007); 
	LCD_WriteReg(0x0031,0x0707);   
	LCD_WriteReg(0x0032,0x0006);
	LCD_WriteReg(0x0035,0x0704);
	LCD_WriteReg(0x0036,0x1f04); 
	LCD_WriteReg(0x0037,0x0004);
	LCD_WriteReg(0x0038,0x0000);        
	LCD_WriteReg(0x0039,0x0706);     
	LCD_WriteReg(0x003c,0x0701);
	LCD_WriteReg(0x003d,0x000f);
	delay_ms(50);  /* delay 50 ms */		
	LCD_WriteReg(0x0050,0x0000);
	LCD_WriteReg(0x0051,0x00ef);
	LCD_WriteReg(0x0052,0x0000);
	LCD_WriteReg(0x0053,0x013f);
	LCD_WriteReg(0x0060,0xa700);
	LCD_WriteReg(0x0061,0x0001);
	LCD_WriteReg(0x006a,0x0000);
	LCD_WriteReg(0x0080,0x0000);
	LCD_WriteReg(0x0081,0x0000);
	LCD_WriteReg(0x0082,0x0000);
	LCD_WriteReg(0x0083,0x0000);
	LCD_WriteReg(0x0084,0x0000);
	LCD_WriteReg(0x0085,0x0000);
	LCD_WriteReg(0x0090,0x0010);
	LCD_WriteReg(0x0092,0x0000);
	LCD_WriteReg(0x0093,0x0003);
	LCD_WriteReg(0x0095,0x0110);
	LCD_WriteReg(0x0097,0x0000);
	LCD_WriteReg(0x0098,0x0000);
	/* display on sequence */
	LCD_WriteReg(0x0007,0x0133);
	LCD_WriteReg(0x0020,0x0000);
	LCD_WriteReg(0x0021,0x0000);
	delay_ms(50);
}

void lcd_clear(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color){
	int i, j;
	lcd_setClippingRegion(x1, y1, x2, y2);
	LCD_SetCursor(x1, y1);
	LCD_WriteIndex(0x0022);
	if (lcd_pattern){
		#if  (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
		for(i = x1; i < x2; ++i){
			for(j = y1; j < y2; ++j)
				LCD_WriteData(lcd_pattern(i, j));
		}
		#elif  (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)
		for(j = y1; j < y2; ++j){
			for(i = x1; i < x2; ++i)
				LCD_WriteData(lcd_pattern(i, j));
		}
		#endif
	}else{
		for(i = 0; i < (x2 - x1) * (y2 - y1); ++i)
			LCD_WriteData(color);
	}
	lcd_unsetClippingRegion();
}

void lcd_clear_all(uint16_t color){
	lcd_clear(0, 0, MAX_X, MAX_Y, color);
}

void lcd_setClippingRegion(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	uint16_t temp;
	#if  (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
	temp = MAX_X - x2;
	x2 = MAX_X - 1 - x1;
	x1 = temp;
 	temp = x1;
	x1 = y1;
	y1 = temp;
	temp = x2;
	x2 = y2 - 1;
	y2 = temp;
	#endif
	LCD_WriteIndex(0x0050);
	LCD_WriteData(x1);
	LCD_WriteIndex(0x0051);
	LCD_WriteData(x2);
	LCD_WriteIndex(0x0052);
	LCD_WriteData(y1);
	LCD_WriteIndex(0x0053);
	LCD_WriteData(y2);
}

void lcd_unsetClippingRegion(void){
	LCD_WriteIndex(0x0050);
	LCD_WriteData(0);
	LCD_WriteIndex(0x0051);
	LCD_WriteData(239);
	LCD_WriteIndex(0x0052);
	LCD_WriteData(0);
	LCD_WriteIndex(0x0053);
	LCD_WriteData(319);
}

void lcd_drawPoint(int16_t x, int16_t y, uint16_t color){
	if (x < 0 || y < 0 || x >= MAX_X || y >= MAX_Y)
		return;
	LCD_SetCursor(x, y);
	LCD_WriteIndex(0x0022);
	if (lcd_pattern)
		LCD_WriteData(lcd_pattern(x, y));
	else
		LCD_WriteData(color);
}

void lcd_drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color){
	int d, dx, dy, ai, bi, xi, yi;
	int x = x1, y = y1;
	if (x1 < x2){ 
		xi = 1;
		dx = x2 - x1;
	}else{ 
		xi = -1;
		dx = x1 - x2;
	}
	if (y1 < y2){ 
		yi = 1;
		dy = y2 - y1;
	}else{ 
		yi = -1;
		dy = y1 - y2;
	}
	lcd_drawPoint(x, y, color);
	if (dx > dy){
		ai = (dy - dx) * 2;
		bi = dy * 2;
		d = bi - dx;
		while (x != x2){ 
			if (d >= 0){ 
				x += xi;
				y += yi;
				d += ai;
			}else{
				d += bi;
				x += xi;
			}
			lcd_drawPoint(x, y, color);
		}
	}else{ 
		ai = ( dx - dy ) * 2;
		bi = dx * 2;
		d = bi - dy;
		while (y != y2){ 
			if (d >= 0){ 
				x += xi;
				y += yi;
				d += ai;
			}else{
				d += bi;
				y += yi;
			}
			lcd_drawPoint(x, y, color);
		}
	}
}

void lcd_printChar(uint16_t x, uint16_t y, char character, uint16_t charColor, uint16_t bkgColor){
	uint16_t i, j;
	uint8_t buffer[16], tmp_char;
	GetASCIICode(buffer, character);
	for(i = 0; i < 16; ++i){
		tmp_char = buffer[i];
		for( j=0; j<8; j++ ){
			if((tmp_char >> (7 - j)) & 0x01)
				lcd_drawPoint(x + j, y + i, charColor);
			else
				lcd_drawPoint(x + j, y + i, bkgColor);
		}
	}
}

void lcd_printText(uint16_t x, uint16_t y, const char *str, uint16_t charColor, uint16_t bkgColor){
	uint8_t i = 0;
	while (*str != '\0')
		lcd_printChar(x + (i++) * 8, y, *(str++), charColor, bkgColor);
}

void lcd_printNText(uint16_t x, uint16_t y, const char *str, uint8_t n, uint16_t charColor, uint16_t bkgColor){
	uint8_t i = 0, white_only = 0;
	while (i < n){
		if (!white_only && !*str)
			white_only = 1;
		lcd_printChar(x + (i++) * 8, y, white_only ? ' ' : *(str++), charColor, bkgColor);
	}
}
