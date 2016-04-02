#ifndef AD_PROBER_H
#define AD_PROBER_H

#include "LPC17xx.h"
#include "PIN_LPC17xx.h"


/** ERROR -- nie zostala wprowadzona ich obsluga
131 - nieprzewidziana czestotliwosc zrodla zegarowego
132 - funkcja nie zostala jeszcze zaimplementowana
133 - nieznany tryb AD
135 - nieznany kanal AD
**/

// etykieta trybu (12.5MHz/33.3MHz)
typedef enum AD_F_pro_t{
	NORM = 0,
	FAST
}AD_F_t;

// etykieta kanalu
typedef enum AD_channel_pro_t{
	AD03 = 3,
	AD05 = 5
}AD_channel_t;

// inicjalizacja ADC, podlaczenie zegara, wyzwalacza, przerwania, kanalu, konfiguracja pinow
uint8_t ad_prober_init(AD_F_t mode);

// wybor kanalu (przelaczenie kanalu)
uint8_t ad_channel_plug(AD_channel_t);

//odlaczenie kanalu
void osc_ad_unplug(void);

// reinicjalizacja ADC
void ad_reinit(void);

static struct{
	AD_F_t mode;
	AD_channel_t current_channel;
	double trig_level;
	bool chn_act[8];
	uint8_t(*init)(AD_F_t);
	uint8_t(*plug)(AD_channel_t);
	void(*unplug)(void);
	void(*reinit)(void);
	uint16_t anomalies_height_limit1;
	uint16_t anomalies_height_limit2;
}osc_ad_prober = {NORM, AD03, 0,
								 {false, false, false, true, false, false, false, false}, 
									ad_prober_init, ad_channel_plug, osc_ad_unplug, ad_reinit, 4096, 4096};
#endif
