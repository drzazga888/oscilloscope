#include "LPC17xx.h"   
#include "PIN_LPC17xx.h"
#include "probe_trig.h"
#include "ad_prober.h"

//wl T/C; bez przerwania; bez znacznika
uint8_t init_TC0_1(uint32_t default_freq){
	
	osc_probe_trig.freq_val = 0;
	LPC_SC->PCONP |= (1<<1); //podlaczenie cclk
	LPC_SC->PCLKSEL0 |= (1<<2); //div na cclk/1
	LPC_TIM0->PR = 0; //inkrementacja licznika bez preskalera
	LPC_TIM0->EMR |= (3<<6); //wyrzucenie sygnalu na zewnatrz poprzez toggle
	LPC_TIM0->MCR |= (1<<4); //autoresetowanie licznika po osiagnieciu match
	LPC_TIM0->TCR |= (1<<0); //wlaczenie licznika

	return osc_probe_trig.set(default_freq); //domyslna czestotliwosc
}

//ustawienie i walidacja znacznika -- dozwolone czestotliwosci ADC
uint8_t set_TC0_1_match(uint32_t freq_val){
	
	uint32_t match_val;
	
	//wbudowane zabezpieczenie gorne czestotliwosci
	if(freq_val > 200000 )
		return 129;
	
	//obliczenie match
	match_val = SystemCoreClock/freq_val;
	
	//ustawienie match
	LPC_TIM0->TC = 0;
	LPC_TIM0->MR1 = match_val;
	
	//aktualizacja czestotliwosci w strukturze
	osc_probe_trig.freq_val = SystemCoreClock/match_val;
	
	return 0;
}

//odlaczenie przerwania, wyl licznika i zrodla zegarowego
void clear_TC0_1(void){
	LPC_TIM0->TCR &= ~(1<<0);
	LPC_SC->PCONP &= ~(1<<1);
}

//podlaczenie przerwania
void plug(void){
	LPC_TIM0->MCR |= (1 << 3); //wl przerwania
	LPC_TIM0->IR |= (1<<1); //ustawienie flagi
}

//odlaczenie przerwania
void unplug(void){
	LPC_TIM0->TCR |= (1<<1);
	LPC_TIM0->MCR &= ~(1 << 3); //wyl przerwania
}

//pobranie nastawionej czestotliwosci
//TODO spr czy uzywasz i jesli czy musisz uzywac...
uint32_t get_frec(void){
	return osc_probe_trig.freq_val;
}

