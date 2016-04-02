#include "osc.h"
#include "../adc/probe_trig.h"
#include "../adc/ad_prober.h"
#include <math.h>
#include "../osc/config.h"

bool meas_done = true;

//wartosc bezwgledna integer
uint32_t dk_abs(int32_t x){
	return (x < 0 ? (uint32_t)(-x) : (uint32_t)(x));
}

//wartosc bezwgledna double 
double dk_fabs(double x){
	return (x < 0 ? (-x) : (x));
}

/*** STATISTICS ***/

//struktura do tablic 'bitowych'
typedef struct{
    uint8_t cell[80];
}bitarray640_t;


//deklaracja tablic
bitarray640_t
    stat_array1,
    stat_array2;


//funkcje pomocnicze get, set tablic bitowych
bool stat_get(bitarray640_t array, uint16_t index){
    return ((array.cell[index/8]) & (1<<(index-(index/8)*8)));
}

void stat_set(bitarray640_t *array, uint16_t index, bool value){
    if(value)
        ((*array).cell[index/8]) |= (1<<(index-(index/8)*8));
    else
        ((*array).cell[index/8]) &= ~(1<<(index-(index/8)*8));
}

//funkcja pomocnicza, szukajaca wszystkich wyzwalaczy na przebiegu
//zapisuje informacje w tablicy bitowej
void digital_trig(uint16_t level, enum TRIGGER_MODE mode){
	uint16_t i;
	
	if( mode == RISING_EDGE ){
		for(i=1;i<config.n;++i)
			if(config.channel[CH1].src[i-1]<level&&config.channel[CH1].src[i]>level)
				stat_set(&stat_array1, i, true);
			else
				stat_set(&stat_array1, i, false);
	}
	else if( mode == FALLING_EDGE ){
		for(i=1;i<config.n;++i)
			if(config.channel[CH1].src[i-1]>level&&config.channel[CH1].src[i]<level)
				stat_set(&stat_array1, i, true);
			else
				stat_set(&stat_array1, i, false);
				
	}
	
	if( mode == RISING_EDGE ){
		for(i=1;i<config.n;++i)
			if(config.channel[CH2].src[i-1]<level&&config.channel[CH2].src[i]>level)
				stat_set(&stat_array2, i, true);
			else
				stat_set(&stat_array2, i, false);
	}
	else if( mode == FALLING_EDGE ){
		for(i=1;i<config.n;++i)
			if(config.channel[CH2].src[i-1]>level&&config.channel[CH2].src[i]<level)
				stat_set(&stat_array2, i, true);
			else
				stat_set(&stat_array2, i, false);
	}
	
}

struct stats_t stats1, stats2;

//ciezszy algorytm, zyskiem pamieci
void statistics(enum CHANNEL channel, struct stats_t *stat, bool is_dev, bitarray640_t *array){
	uint32_t freq = osc_probe_trig.get_frec()/2.0;
	uint16_t* data = config.channel[channel].src; 

	uint16_t 
		i, ip, ik, n_period=0, i_pop,
		max_vol=0, min_vol=4096, max_amp=0, min_amp=4096, amp;
	
	double 
		dev_sum_amp=0, dev_sum_f=0,
		max_f=0, min_f=200000, curr_freq;
	
	uint32_t 
		amp_sum=0;
		
	
	//generowanie tablicy pol bitowych (czy wyzwalacz)
	digital_trig((uint16_t)(sci_get(config.trig_level)/3.3*4096), config.trigger_mode);
	
	
	//petla po polach bitowych, szukanie pierwszego wyzwalacza
	for(i=0;i<config.n;++i)
		if( stat_get((*array), i) ){
			ip = i;
			//++n_period;
			break;
		}
	
	//zrezygnowanie ze statystyki dla mniej niz 3 okresow
	if(i>=config.n/3){
		(*stat).avr_f=0;
		(*stat).dev_f=0;
		(*stat).max_f=0;
		(*stat).min_f=0;
		(*stat).avr_amp=0;
		(*stat).max_amp=0;
		(*stat).min_amp=0;
		(*stat).dev_amp=0;
		return;
	}
			
	//petla zasadnicza po polach bitowych, 
	//szukanie ostatniego wywalacza
	//szukanie amplitudy sr, max, min
	//oraz czestotliwosci max, min
	i_pop = ip;
	for(i=ip+1;i<config.n;++i){
		//szukanie max
		if( data[i]>max_vol )
				max_vol = data[i];
		//szukanie min
		if( data[i]<min_vol )
				min_vol = data[i];
		//na wyzwalacz
		if( stat_get((*array), i) ){
			//szukanie ostatniego wywalacza
			ik = i;	
			++n_period;
			//zapisanie amp, zapisanie jesli ekstremum
			amp = max_vol-min_vol;
			amp_sum+=amp;
			
			if(amp>max_amp)
				max_amp = amp;
			if(amp<min_amp)
				min_amp = amp;
			
			//zerowanie max, min_vol
			max_vol = 0;
			min_vol = 4096;
			
			//zapisanie f, zapisanie jesli ekstremum
			curr_freq = (double)freq/(i-i_pop);
			
			if(curr_freq>max_f)
				max_f=curr_freq;
			if(curr_freq<min_f)
				min_f=curr_freq;
			
			//zapamietanie poprzedniego wyzwalacza
			i_pop = i;
		}
	}
		
	//zrezygnowanie jesli ostatni wyzwalacz jest pierwszym
	if(n_period==1) return;
		
	//przypisanie obliczone sredniej czestotliowosci, amplitudy, amp. max i min
	(*stat).avr_f = (double)n_period*freq/(ik-ip);
	(*stat).avr_amp = (double)amp_sum/n_period/4096*3.3;
	(*stat).max_amp = (double)max_amp/4096*3.3;
	(*stat).min_amp = (double)min_amp/4096*3.3;
	(*stat).max_f = max_f;
	(*stat).min_f = min_f;
	
		
	if(is_dev){
		//petla wtorna do obliczania odchylen jesli sa potrzebne
		i_pop = ip;
		for(i=ip+1;i<=ik;++i){
			//szukanie max
			if( data[i]>max_vol )
					max_vol = data[i];
			//szukanie min
			if( data[i]<min_vol )
					min_vol = data[i];
			//na wyzwalacz
			if( stat_get((*array), i) ){
				amp = max_vol-min_vol;
				dev_sum_amp+=dk_fabs(amp-(*stat).avr_amp)/n_period;
				dev_sum_f+=dk_fabs(((double)freq/(i-i_pop))-(*stat).avr_f);
				i_pop = i;
			}			
		}
		(*stat).dev_amp=dev_sum_amp/n_period/4096*3.3;
		(*stat).dev_f=dev_sum_f/n_period;
	}
}

/** KONIEC STATISTICS **/

//funkcja pomocnicza wywolujaca statystyki te ktore potrzebne
void osc_statistics(){
	if(stats_enabled){	
		switch(stats_target){
			case STATS_CH1:
				statistics(CH1, &stats1, true, &stat_array1);
			break;
			case STATS_CH2:
				statistics(CH2, &stats2, true, &stat_array2);
			break;
			case STATS_AVR_DEV:
				statistics(CH1, &stats1, true, &stat_array1);
				statistics(CH2, &stats2, true, &stat_array2);
			break;
			case STATS_MAX_MIN:
				statistics(CH1, &stats1, false, &stat_array1);
				statistics(CH2, &stats2, false, &stat_array2);
			break;
		}
	}
}

//inicjalizacja sprzetowa czesci pomiarowej oscyloskopu
void osc_init(void){
	osc_probe_trig.init(freq()*2);
	osc_ad_prober.init(NORM);
}

//bardzo niebezpieczna operacja
void osc_meas(void){
	SCB->SCR |= (1<<1);
	osc_ad_prober.reinit();
	__WFI();
	while(!meas_done);
}

//funkcja wspierajaca szukanie wyzwalacza cyfrowego (jednego)
uint16_t osc_digital_trig(uint16_t* data, uint16_t level, enum TRIGGER_MODE mode){
	uint16_t i;
	if( mode == RISING_EDGE ){
		for(i=config.n/2;i>0;--i)
			if(data[i-1]<level&&data[i]>level)
				return i;
	}
	else if( mode == FALLING_EDGE ){
		for(i=config.n/2;i>0;--i)
			if(data[i-1]>level&&data[i]<level)
				return i;
	}
	return MAX_X / 2;
}



