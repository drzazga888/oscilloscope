#ifndef GUI_H
#define GUI_H

#include "LPC17xx.h"             
#include "../lcd/lcd.h"
#include "chart.h"
#include "../utils/caster.h"
#include "gui_controller.h"

// rozmiar fonta

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

// kolory

#define GUI_COLOR_BKG RGB(20, 20, 20)
#define GUI_COLOR_FONT White
#define GUI_COLOR_HOVER RGB(255, 127, 0)
#define GUI_COLOR_DISABLED RGB(100, 100, 100)
#define GUI_COLOR_ACCENT1 Yellow
#define GUI_COLOR_ACCENT2 RGB(0, 230, 255)

// przyciski

enum GUI_BUTTON {
	GUI_KEY1,
	GUI_KEY2,
	GUI_INT0
};

// czasy oczekiwania

#define GUI_BUTTON_KEY1_TIMEOUT 300
#define GUI_BUTTON_KEY2_TIMEOUT 300
#define GUI_BUTTON_INT0_TIMEOUT 300
#define GUI_JOY_TIMEOUT 600
#define GUI_TITLE_TIMEOUT 5000
#define GUI_IRQ_TIME_DECREMENT 100

// bufory znakowe

enum GUI_AREA {
	GUI_UP1,
	GUI_UP2,
	GUI_UP3,
	GUI_UP4,
	GUI_DOWN1,
	GUI_DOWN2,
	GUI_DOWN3,
	GUI_DOWN4,
	GUI_NONE
};

// funkcje

// inicjalizacje wszystkich elementow gui
void gui_init(void);

// przerysowanie elementow gui, ktore zostaly zmienione
void gui_redraw(void);

// reakcja elementow gui na joystick
void gui_react_to_joy(void);

// zwiekszanie wartosci struktury Sci na podstawie czasu,
// przez jaki trzymany byl joystick
void gui_sci_increase(Sci *sci, int8_t lim_man, int8_t lim_exp);

// zmniejszenie wartosci struktury Sci na podstawie czasu,
// przez jaki trzymany byl joystick
void gui_sci_decrease(Sci *sci, int8_t lim_man, int8_t lim_exp);

// bufory znakowe

extern char gui_up_buffer[40];
extern char gui_down_buffer[4][10];
extern uint8_t gui_need_redraw[8];
extern uint16_t gui_area_colours[8];

// przechowywanie aktywnej opcji

extern enum GUI_AREA gui_selected;

// blokowanie wchodzenia do gornego menu

extern bool gui_top_locked;

extern int16_t gui_joy_active_time;

#endif
