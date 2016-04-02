#ifndef __CHART_H
#define __CHART_H

#include "../lcd/lcd.h"
#include "../osc/config.h"
#include "gui.h"

// defines

#define DUMMY_Y 255
#define START_Y 16

// functions

// inicjalizacja ekranu
void chart_init(void);

// szablon do malowania kropkowanej siatki
uint16_t chart_sub_mesh(uint16_t x, uint16_t y);

// szablon do malowania calego tla wyswietlacza
// - gorne i dolne tlo paskow gui
// - siatka pod oscylogram
uint16_t chart_mesh(uint16_t x, uint16_t y);

// rysowanie samego wykresu
void chart_draw(void);

// ustawianie pozycji strzalki wskazujacej napiecie wyzwalania triggera
void chart_set_trig(void);

// rysowanie strzalki triggera na wyswietlacz
void chart_draw_trig(void);

#endif
