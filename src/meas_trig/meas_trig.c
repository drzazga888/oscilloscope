#include "LPC17xx.h"   
#include "PIN_LPC17xx.h"
#include "meas_trig.h"
#include "../adc/probe_trig.h"

void meas_trig_init(void){
	NVIC_EnableIRQ(EINT3_IRQn);
}

void meas_trig_unable(void){
	PIN_Configure(0, 26, 0, 2, 0);
	LPC_GPIOINT->IO0IntEnR |= (1<<26);
	LPC_GPIOINT->IO0IntClr |= (1<<26);
}

void meas_trig_disable(void){
	LPC_GPIOINT->IO0IntEnR &= ~(1<<26);
	PIN_Configure(0, 26, 1, 2, 0);
}

extern uint16_t flag;

/*
void EINT3_IRQHandler(void){
	osc_meas_trig.disable();
	osc_probe_trig.unfreeze();
}
*/