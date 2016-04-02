#ifndef __LCD_H
#define __LCD_H

#define PIN_EN		(1 << 19)
#define PIN_LE		(1 << 20)
#define PIN_DIR		(1 << 21)
#define PIN_CS    (1 << 22)
#define PIN_RS		(1 << 23)
#define PIN_WR		(1 << 24)
#define PIN_RD		(1 << 25)   

#define LCD_EN(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_EN) : (LPC_GPIO0->FIOCLR = PIN_EN));
#define LCD_LE(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_LE) : (LPC_GPIO0->FIOCLR = PIN_LE));
#define LCD_DIR(x)  ((x) ? (LPC_GPIO0->FIOSET = PIN_DIR) : (LPC_GPIO0->FIOCLR = PIN_DIR));
#define LCD_CS(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_CS) : (LPC_GPIO0->FIOCLR = PIN_CS));
#define LCD_RS(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_RS) : (LPC_GPIO0->FIOCLR = PIN_RS));
#define LCD_WR(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_WR) : (LPC_GPIO0->FIOCLR = PIN_WR));
#define LCD_RD(x)   ((x) ? (LPC_GPIO0->FIOSET = PIN_RD) : (LPC_GPIO0->FIOCLR = PIN_RD));

#define DISP_ORIENTATION 90

#if  (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
#define  MAX_X  320
#define  MAX_Y  240
#elif  (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)
#define  MAX_X  240
#define  MAX_Y  320
#endif

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define RGB(red, green, blue)\
	(uint16_t)( (( red   >> 3 ) << 11 ) | \
	(( green >> 2 ) << 5  ) | \
	( blue  >> 3 ))
	
// globals
extern uint16_t (*lcd_pattern)(uint16_t, uint16_t);

// inicjalizacja wyswietlacza lcd
void lcd_init(void);

// czyszczenie fragmentu ekranu wybranym kolorem
// (badz szablonem, o ile ustawiona jest
// zmienna lcd_pattern) - wtedy wybrany
// kolor jest pomijany w rysowaniu
void lcd_clear(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

// czyszczenie calego ekranu wybranym kolorem
// (badz szablonem, o ile ustawiona jest
// zmienna lcd_pattern) - wtedy wybrany
// kolor jest pomijany w rysowaniu
void lcd_clear_all(uint16_t color);

// ustawia obszar, w ktorym mozna rysowac. Wyjscie
// poza ten obszar skutkuje pominieciem rysowania
// piksela. Funckja musi byc wywolana przez
// wywolaniem funckji lcd_clear.
// Funkcja znacznie przyspiesza czyszczenie ekranu
// dzieki skorzystaniu z faktu mianowice
// sprzetowe zablokowanie ekranu nadal pozwala
// na podanie tylko raz kursora - kolejne piksele
// beda przesuwane od lewej do prawej, z gory na dol
// (kierunek czytania)
void lcd_setClippingRegion(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

// usuwanie blokowania fragmenu ekranu
void lcd_unsetClippingRegion(void);

// rysowanie piksela
void lcd_drawPoint(int16_t x, int16_t y, uint16_t color);

// rysowanie linii
void lcd_drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

// rysowanie litery
void lcd_printChar(uint16_t x, uint16_t y, char character, uint16_t charColor, uint16_t bkgColor);

// drukowanie tekstu na ekranie w podanej pozycji
void lcd_printText(uint16_t x, uint16_t y, const char *str, uint16_t charColor, uint16_t bkgColor);

// drukowanie tekstu z uwzglednieniem ilosci znakow
// do zmiany - zawsze podana liczba znakow zostanie zmieniona,
// nawet jesli skonczy sie bufor (wtedy wypisywane beda spacje)
void lcd_printNText(uint16_t x, uint16_t y, const char *str, uint8_t n, uint16_t charColor, uint16_t bkgColor);

#endif
