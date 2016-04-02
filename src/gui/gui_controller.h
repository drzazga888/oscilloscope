#ifndef __GUI_CONTROLLER
#define __GUI_CONTROLLER

#include "gui.h"

enum GUI_OPTION {
	GUI_NO_OPTION,
	GUI_OUTPUT_MODE,
	GUI_SINGLE_MEAS,
	GUI_TRIGGER,
	GUI_MEDIAN_FILTER_CH1,
	GUI_MEDIAN_FILTER_CH2,
	GUI_STATS
};

#define GUI_OPTION_SIZE 7
#define DX 3

// reakcja na wcisniecie przycisku INT0
void gui_int0(void);

// reakcja na wcisniecie przycisku KEY1
void gui_key1(void);

// reakcja na wcisniecie przycisku KEY2
void gui_key2(void);

// reakcja na wychylenie joysticka w gore
void gui_joy_up(void);

// reakcja na wychylenie joysticka w dol
void gui_joy_down(void);

// reakcja na wychylenie joysticka w lewo
void gui_joy_left(void);

// reakcja na wychylenie joysticka w prawo
void gui_joy_right(void);

// reakcja na wcisniecie joysticka
void gui_joy_center(void);

// odtwarza wszystkie bufory znakowe
void regenerate(void);

// odtwarza fragment buforu dornego paska,
// zapisujac w odpowiednim miejscu wartosc dt
void generate_dt(void);

// odtwarza fragment buforu dornego paska,
// zapisujac w odpowiednim miejscu wartosc dv
void generate_dv(void);

// odtwarza fragment buforu dornego paska,
// zapisujac w odpowiednim miejscu wartosc
// zwiazana z kanalem 1
void generate_ch1(void);

// odtwarza fragment buforu dornego paska,
// zapisujac w odpowiednim miejscu wartosc
// zwiazana z kanalem 2
void generate_ch2(void);

// odtwarza bufor tekstowy dolnego paska gui
// zapisujac do niego informacje o aktualnie
// wyswietlanej opcji
void generate_options(void);

// odtwarza bufor tekstowy dolnego paska gui
// zapisujac do niego informacje o aktualnie
// wyswietlanych statystykach wykresow
void generate_stats(void);

#endif
