#ifndef CASTER_H
#define CASTER_H

#include <string.h>
#include "../osc/config.h"

#define UNIT_3 "k"
#define UNIT_3_LENGTH 1

#define UNIT_0 ""
#define UNIT_0_LENGTH 0

#define UNIT_MINUS_3 "m"
#define UNIT_MINUS_3_LENGTH 1

#define UNIT_MINUS_6 "u"
#define UNIT_MINUS_6_LENGTH 1

#define UNIT_MINUS_9 "n"
#define UNIT_MINUS_9_LENGTH 1

#define UNIT_PRESCALER 3
#define UNIT_OFFSET 1

// funkcja sluzy do odwracania napisu
void reverse(char *s, int length);

// funkcja przeksztalca liczbe typu int
// na rezprezentacje znakowa
int itoa(int n, char s[]);

// zamiana liczby zmiennoprzecinkowej na napis
// z okreslona liczba cyfr po przecinku
int ftoa(float n, char s[], char precision);


// zamiana float na napis z okreslona prezyzja.
// Na koniec napisu doklejony zostaje dopisana jednostka
int ftoa_units(float n, char s[], char precision);

// funkcja zamieniajaca strukture Sci na reprezentacje
// znakowa z odpowiednia jednostka na koncu
int scitoa(Sci val, char *text, char precision);

#endif
