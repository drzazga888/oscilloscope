#include "LPC17xx.h"   
#include "PIN_LPC17xx.h"
#include "ad_prober.h"
#include "probe_trig.h"
#include "../osc/config.h"
#include "../osc/osc.h"

#define MEDIAN_DEGREE (1)
#define ADC3_RESULT (((LPC_ADC->ADDR3)&65520)>>4) 
#define ADC5_RESULT (((LPC_ADC->ADDR5)&65520)>>4) 

//kursor danych
uint32_t n = 0;
bool is_trig = 0;

uint8_t ad_prober_init(AD_F_t mode){
	if(SystemCoreClock != 100000000) //test na 100MHz
		return 131;
	
	LPC_SC->PCONP |= (1<<12); //podlaczenie cclk
	LPC_ADC->ADCR |= (1<<21); //wl modulu ADC
	LPC_ADC->ADINTEN &= ~(1<<8); //wl indiwidualnych rejestrow na odbior danych
	
	if(osc_ad_prober.mode == NORM) // [12.5MHZ]
		LPC_SC->PCLKSEL0 |= ( (1<<25) | (1<<24) ); //div1 cclk/8 
	else if(osc_ad_prober.mode == FAST){ // [33.3MHZ]
		LPC_SC->PCLKSEL0 &= ~(1<<25); 
		LPC_SC->PCLKSEL0 |= (1<<24); //div1 cclk/1
		LPC_ADC->ADCR |= (3<<8); //div2 adclk/3
	}
	else
		return 133;
	
	NVIC_EnableIRQ(ADC_IRQn); //przerwanie konca pomiaru
	LPC_ADC->ADINTEN |= (1<<3) | (1<<5); //koniec wywoluje przerwanie
	
	PIN_Configure(0, 26, 1, 2, 0); //podlaczenie AD03 (speaker)
	PIN_Configure(1, 31, 3, 2, 0); //podlaczenie AD05 (potentiometer)
	LPC_ADC->ADCR |= (1<<26); //wywolanie na MATCH0.1
	return 0;
}

uint8_t ad_channel_plug(AD_channel_t channel){
	
	osc_ad_prober.unplug();
	if(channel == AD03){ //podlaczenie pinu do AD (multiplekser)
		LPC_ADC->ADCR |= (1<<3); 
		return 0;
	}
	else if(channel == AD05){
		LPC_ADC->ADCR |= (1<<5);
		return 0;
	}
	else
		return 135;		
}

void osc_ad_unplug(void){ //odlaczenie wszystkich kanalow
	LPC_ADC->ADCR &= ~0xFF;
}

void ad_reinit(void){
	uint8_t chn;
	//zerowanie kursora
	n=0;
	
	//zmiana kanalu
	if(meas_done){
		meas_done = false;
		is_trig = false;
		chn = 0;
	}
	else{
		++osc_ad_prober.current_channel;
		chn = (uint8_t)osc_ad_prober.current_channel;
	}
	while(true){
		//obsluga konca pomiaru wszystkich kanalow
		if(chn == 8){
			meas_done = true;
			//zmiana czestotliwosci
			osc_probe_trig.set(freq()*2);
			//zmiany filtrow
			osc_ad_prober.anomalies_height_limit1 = sci_get(config.channel[CH1].anomalies_height_limit) / 3.3 * 4096;
			osc_ad_prober.anomalies_height_limit2 = sci_get(config.channel[CH2].anomalies_height_limit) / 3.3 * 4096;
			//zmiana triggera
			osc_ad_prober.trig_level = (uint16_t)(sci_get(config.trig_level)/3.3*4096);
			//zmiany kanalow (dozwolonych)
			if(config.channel[CH1].is_active)
				osc_ad_prober.chn_act[AD03] = 1;
			else 
				osc_ad_prober.chn_act[AD03] = 0;
			if(config.channel[CH2].is_active)
				osc_ad_prober.chn_act[AD05] = 1;
			else 
				osc_ad_prober.chn_act[AD05] = 0;
			return;
		}
		//spr czy dozwolony kanal
		if(osc_ad_prober.chn_act[chn]){
			if(chn==AD03 || chn==AD05){
				osc_ad_prober.plug((AD_channel_t)chn);
				osc_ad_prober.current_channel = (AD_channel_t)chn;
				break;
			}
		}
		++chn;
	}
}

void median_filter_p( uint16_t *tabi, uint16_t **tabo, int16_t indx, uint8_t degree, uint16_t limit){
	//czy da sie przeprowadzic (zakres danych w tablicy)
	if(!((signed)(indx-degree)<0 || indx+degree>config.n)){
        uint16_t i;
        uint16_t avr=0, avr_neighbourhood;
        uint16_t dis_degree = degree-1;

        //srednia zaklocenia
        //1. stopien -- 1 pkt
        //2. stopien -- 3 pkt itd...
        for(i=indx-dis_degree;i<=indx+dis_degree;++i)
            avr+=tabi[i];
        avr/=2*dis_degree+1;

        //srednia wartosc pkt otoczenia
        avr_neighbourhood = (tabi[indx-degree] + tabi[indx+degree]) / 2;

        //warunek filtrowania -- dwa
        //oddzielnie prawy i lewy pkt obszaru zaklocenia
        //przeciwdziala filtracjom zbocza
				if(dk_abs(tabi[indx-degree]-avr)>limit && dk_abs(tabi[indx+degree]-avr)>limit){
					(*tabo)[indx]=avr_neighbourhood;
        }
    }	
}

//funkcja wlasnego sprawdzajac czy punkt jest triggerem
bool ad_digital_trig(uint16_t* data, uint16_t level, enum TRIGGER_MODE mode, uint16_t id){
	if(id<1) return 0;
	if( mode == RISING_EDGE ){
			if(data[id-1]<level&&data[id]>level)
				return 1;
	}
	else if( mode == FALLING_EDGE ){
			if(data[id-1]>level&&data[id]<level)
				return 1;
	}
	return 0;
}

void ADC_IRQHandler(void){	
		//obsluga zapelnienia tablicy
	if(n>=config.n){ //obsluga wylaczenia pomiaru
		SCB->SCR &= ~(1<<1); //wyl SLEEPONEXIT
		osc_ad_prober.unplug(); //odlaczenie przerwania

		//odebranie wyniku -- zerowanie przerwania
		if(osc_ad_prober.current_channel == AD03)
			LPC_ADC->ADDR3;
		else if(osc_ad_prober.current_channel == AD05)
			LPC_ADC->ADDR5;
		
		//reinicjalizacja
		osc_ad_prober.reinit();	
	}
	else{
		//odebranie wyniku, wyluskanie z rejestru
		if(osc_ad_prober.current_channel == AD03){
			config.channel[CH1].src[n] = ADC3_RESULT;
		}
		else if(osc_ad_prober.current_channel == AD05){
			config.channel[CH2].src[n] = ADC5_RESULT;
		}
		
		//filtrowanie medianowe w przerwaniu
		if (config.channel[CH1].median_on)
			median_filter_p(config.channel[CH1].src, &(config.channel[CH1].src), n-MEDIAN_DEGREE, MEDIAN_DEGREE, osc_ad_prober.anomalies_height_limit1);
		if (config.channel[CH2].median_on)
			median_filter_p(config.channel[CH2].src, &(config.channel[CH2].src), n-MEDIAN_DEGREE, MEDIAN_DEGREE, osc_ad_prober.anomalies_height_limit2);
		
		++n;
		
		//sprawdzenie czy pkt jest triggerem
		if(!is_trig){
			if(osc_ad_prober.current_channel == AD03){
				if(ad_digital_trig(config.channel[CH1].src, osc_ad_prober.trig_level, config.trigger_mode, n))
					config.channel[CH1].src[0] = config.channel[CH1].src[n];
				is_trig = true;
				n=1;
			}
			else if(osc_ad_prober.current_channel == AD05){
				if(ad_digital_trig(config.channel[CH2].src, osc_ad_prober.trig_level, config.trigger_mode, n))
					config.channel[CH2].src[0] = config.channel[CH2].src[n];
				n=1;
				is_trig = true;
			}
		}
	}	
}
	


