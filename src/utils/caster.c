#include "caster.h"

const char * const units[] = {
	UNIT_3,
	UNIT_0,
	UNIT_MINUS_3,
	UNIT_MINUS_6,
	UNIT_MINUS_9
};

const uint8_t units_lengths[] = {
	UNIT_3_LENGTH,
	UNIT_0_LENGTH,
	UNIT_MINUS_3_LENGTH,
	UNIT_MINUS_6_LENGTH,
	UNIT_MINUS_9_LENGTH
};

void reverse(char *s, int length){
    char *j;
    int c;
    j = s + length - 1;
    while(s < j) {
        c = *s;
        *s++ = *j;
        *j-- = c;
    }
}

int itoa(int n, char s[]){
    int i, sign;
    if ((sign = n) < 0)
    n = -n;
    i = 0;
    do
        s[i++] = n % 10 + '0';
    while ((n /= 10) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s, i);
    return i;
}

int ftoa(float n, char s[], char precision){
    int char_n, total = 1 + (n < 1 && n > -1);
    char i = precision;
    float n_prec = n;
    while (i-- > 0)
        n_prec *= 10;
    total += itoa(n_prec, s + total);
    if (n < 0 && n > -1){
        s[0] = '-';
        s++;
    }
    char_n = itoa(n, s);
    s[char_n] = '.';
    return total;
}

int ftoa_units(float n, char s[], char precision){
	int f_len;
	int8_t unit = 0;
	if (n != 0.0){
		while (n >= 1000.0){
			n /= 1000.0;
			--unit;
		}
		while (n < 0.1){
			n *= 1000.0;
			++unit;
		}
	}
	f_len = ftoa(n, s, precision);
	if (f_len > 5)
		f_len = 5;
	strcpy(s + f_len, units[unit + UNIT_OFFSET]);
	return f_len + units_lengths[unit + UNIT_OFFSET];
}

int scitoa(Sci val, char *text, char precision){
	int f_len;
	float to_show = val.man;
	while (val.exp % UNIT_PRESCALER != 0){
		to_show *= 0.1;
		++val.exp;
	}
	f_len = ftoa(to_show, text, precision);
	strcpy(text + f_len, units[val.exp / -UNIT_PRESCALER + UNIT_OFFSET]);
	return f_len + units_lengths[val.exp / -UNIT_PRESCALER + UNIT_OFFSET];
}
