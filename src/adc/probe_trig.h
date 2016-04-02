#include "LPC17xx.h"

/** ERROR
129 - przekorczenie gornego zakresu ustawienia
**/

#ifndef PROBE_TRIG
#define PROBE_TRIG

//deklaracje funkcji wewnetrznych 
uint8_t init_TC0_1(uint32_t default_freq);
uint8_t set_TC0_1_match(uint32_t);
//void TC0_1_unable(void);
//void TCO_1_disable(void);
void clear_TC0_1(void);
void plug(void);
void unplug(void);
uint32_t get_frec(void);

static struct{
	uint32_t freq_val;
	uint8_t(*init)(uint32_t);
	uint8_t(*set)(uint32_t);
	void(*clear)(void);
	void(*plug)(void);
	void(*unplug)(void);
	uint32_t(*get_frec)(void);
}osc_probe_trig = {0, init_TC0_1, set_TC0_1_match, clear_TC0_1, plug, unplug, get_frec};




#endif //PROBE_TRIG
