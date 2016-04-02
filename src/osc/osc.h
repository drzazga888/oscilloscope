#ifndef OSC_H
#define OSC_H

#include "PIN_LPC17xx.h"
#include "../adc/ad_prober.h"
#include "config.h"

extern bool meas_done;
extern struct stats_t stats1, stats2;

uint32_t dk_abs(int32_t x);

struct stats_t{
	double avr_f;
	double dev_f;
	double max_f;
	double min_f;
	double avr_amp;
	double max_amp;
	double min_amp;
	double dev_amp;
};

void osc_init(void);
uint16_t osc_digital_trig(uint16_t*, uint16_t, enum TRIGGER_MODE);
void osc_meas(void);
void osc_statistics(void);


static struct{
	uint32_t freq;
	void(*init)(void);
	uint16_t (*digital_trig)(uint16_t*, uint16_t, enum TRIGGER_MODE);
	void(*meas)(void);
	void(*statistics)(void);
}osc = {500, osc_init, osc_digital_trig, osc_meas, osc_statistics};

#endif



