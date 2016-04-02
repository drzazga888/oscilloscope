/********************************************************************************
*																																								*
*  ------------------------------ OSTRZEZENIE	--------------------------------	*
*																																								*
*  WSZYSTKIE WARTOSCI W KONFIGURACJI MOGA ZOSTAC ZMIENIANE PRZEZ PRZERWANIA.		*
*  W TRAKCIE OBSLUGI FUNKCJI NALEZY KOPIOWAC ZARTOSCI DO ZMIENNEJ TYMCZASOWEJ		*
*  GDYZ BLEDNE UZYWANIE KONFIGURACJI MOZE USZKODZIC UC, TWOJE ZYCIE							*
*  LUB ZDROWIE !!!																															*
*																																								*
********************************************************************************/

#ifndef __CONFIG_H
#define __CONFIG_H

#include "LPC17xx.h"                    // Device header
#include "Board_Joystick.h"             // ::Board Support:Joystick
#include "../lcd/lcd.h"
#include "../utils/sci.h"
#include "PIN_LPC17xx.h"

// zakresy zmiennosci zmiennych
// patrz: tabelka w dokumentacji

#define MAX_DT_MAN 67
#define MAX_DT_EXP -3

#define MIN_DT_MAN 20
#define MIN_DT_EXP -5

#define MAX_DV_MAN 77
#define MAX_DV_EXP -2

#define MIN_DV_MAN 10
#define MIN_DV_EXP -2

#define MAX_MED_FILT_MAN 33
#define MAX_MED_FILT_EXP -1

#define MIN_MED_FILT_MAN 10
#define MIN_MED_FILT_EXP -3

#define MAX_TRIG_LVL_MAN 33
#define MAX_TRIG_LVL_EXP -1

#define MIN_TRIG_LVL_MAN 10
#define MIN_TRIG_LVL_EXP -3

#define JOY_ROTATED_LEFT JOYSTICK_DOWN
#define JOY_ROTATED_RIGHT JOYSTICK_UP
#define JOY_ROTATED_UP JOYSTICK_LEFT
#define JOY_ROTATED_DOWN JOYSTICK_RIGHT

#define DATA_WIDTH 640

extern uint16_t chn_buff[2][1][DATA_WIDTH];


// enum & structs

// enum do przechowywania aktywnego kanalu oscyloskopa
enum CHANNEL {
	CH1,
	CH2,
	NO_CHANNEL
};

#define TRIGGER_MODE_LENGTH 2

// enum przechowuje mozliwe typy
// wyzwalania triggera (typy zboczy)
enum TRIGGER_MODE {
	FALLING_EDGE,
	RISING_EDGE
};

// konfiguracja konkretnego kanalu
// patrz: tabelka w dokumentacji
struct CHANNEL_CONFIG {
	bool is_active, median_on;
	int32_t x, y;
	uint16_t *src;
	Sci anomalies_height_limit;
};

// enum do peryferiow mikrokontrolera
enum PERIPHERY {
	LCD,
	ADC,
	//SD,
	//USB,
	//IIC
	UART
};

// typy statystyk
enum STATS_TARGET {
	STATS_CH1,
	STATS_CH2,
	STATS_AVR_DEV,
	STATS_MAX_MIN
};

// parametry poddane statystyce
enum STATS_TYPE {
	STATS_FREQ,
	STATS_AMP
};

// globalna konfiguracja oscyloskopa
// patrz: tabelka w dokumentacji
struct CONFIG {
	Sci dv, dt, f, trig_level;
	uint16_t n, trig_cursor;
	bool over, single_meas, snapshot;
	enum PERIPHERY scope;
	enum CHANNEL trigger_channel;
	enum TRIGGER_MODE trigger_mode;
	struct CHANNEL_CONFIG channel[2];
};


// funkcje

// funkcja zwraca ustawiona przez
// uzytkownika czestotliwosc wykonywania
// pomiarow
float freq(void);

// externs

extern struct CONFIG config;

extern bool stats_enabled;
extern enum STATS_TARGET stats_target;
extern enum STATS_TYPE stats_type;

#endif
