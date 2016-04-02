#include <math.h>
#include "config.h"
#include "../adc/ad_prober.h"
#include "PIN_LPC17xx.h"

uint16_t chn_buff[2][1][DATA_WIDTH];

struct CONFIG config;

bool stats_enabled = false;
enum STATS_TARGET stats_target = STATS_CH1;
enum STATS_TYPE stats_type = STATS_FREQ;

//przeliczenie i pobranie dt podzialki wykresu na czestotliwosc pomiarowa
float freq(void){
	return 1.0 / sci_get(config.dt) * 40.0;
}
