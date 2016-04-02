#ifndef __SCI_H
#define __SCI_H

#include "LPC17xx.h"                    // Device header

// struktura, ktora sluzy do przechowywania
// liczby zmiennoprzecinkowej jako dwie liczby
// typu int: mantysa i eksponenta.
// wartosc = man * 10^exp
// Struktura ulatwia zwiekszanie badz
// zmniejszanie wartosci o odpowiednia
// krotnosc. Ponizej znajduje sie przyklad
// zwiekszania / zmniejszania tych wartosci:
// 20 * 10^3
// 21 * 10^3
// ...
// 99 * 10^3
// 10 * 10^4
// 11 * 10^4
// ...
// 99 * 10^4
// 10 * 10^5
// 11 * 10^5
// ...
// Dzieki takiemu postepowaniu zmiana
// wartosci jest adekwatna do rzedu liczby
typedef struct Sci_impl {
    int8_t man;
	  int8_t exp;
} Sci;

// funkcja zwieksza liczbe w Sci n razy (patrz: opis wyzej).
// Uwzgledniany jest tez limit wartosci Sci, ktorego
// nie mozna przekroczyc
void sci_increase_n(Sci *sci, int8_t lim_man, int8_t lim_exp, uint8_t n);

// funkcja zmniejsza liczbe w Sci n razy (patrz: opis struktyry Sci).
// Uwzgledniany jest tez limit wartosci Sci, ktorego
// nie mozna przekroczyc
void sci_decrease_n(Sci *sci, int8_t lim_man, int8_t lim_exp, uint8_t n);

// funkcja inkrementuje Sci (patrz: opis struktyry Sci).
// Uwzgledniany jest tez limit wartosci Sci, ktorego
// nie mozna przekroczyc
void sci_increase(Sci *sci, int8_t lim_man, int8_t lim_exp);

// funkcja dekrementuje Sci (patrz: opis struktyry Sci).
// Uwzgledniany jest tez limit wartosci Sci, ktorego
// nie mozna przekroczyc
void sci_decrease(Sci *sci, int8_t lim_man, int8_t lim_exp);

// ustawianie zmiennej typu Sci na podstawie float-a
void sci_set(Sci *sci, float fl_val);

// pobranie wartosci typu float na podstawie Sci
float sci_get(Sci sci);

#endif
