#include <math.h>
#include <string.h>
#include "PIN_LPC17xx.h"                
#include "GPIO_LPC17xx.h"               
#include "Board_Joystick.h"            
#include "LPC17xx.h"                    
#include "osc/config.h"
#include "gui/chart.h"
#include "osc/osc.h"
#include "comm/uart.h"

uint32_t xnx = 0;

//DEBUG
void run_timer1(){
	LPC_SC->PCONP |= (1<<2); //podlaczenie cclk
	LPC_SC->PCLKSEL0 |= (1<<4); //div na cclk/1
	LPC_TIM1->PR = 0; //inkrementacja licznika bez preskalera
	LPC_TIM1->TCR |= (1<<0); //wlaczenie licznika
}
	
int main(void){	
	//DEBUG
	run_timer1();
	
	// ustawianie wartosci domyslnych
	config.n = 640;
	config.over = true;
	config.channel[CH1].src = chn_buff[CH1][0];
	config.channel[CH2].src = chn_buff[CH2][0];		
	config.dv.man = 60;
	config.dv.exp = -2;
	config.dt.man = 70;
	config.dt.exp = -4;
	config.f.man = 56;
	config.f.exp = 2;
	config.trigger_channel = CH1;
	config.trig_level.man = 10;
	config.trig_level.exp = -1;
	config.trigger_mode = RISING_EDGE;
	config.scope = LCD;
	config.channel[CH1].is_active = 1;
	config.channel[CH2].is_active = 1;
	config.channel[CH1].median_on = true;
	config.channel[CH2].median_on = true;
	config.channel[CH1].anomalies_height_limit.man = 80;
	config.channel[CH1].anomalies_height_limit.exp = -2;
	config.channel[CH2].anomalies_height_limit.man = 80;
	config.channel[CH2].anomalies_height_limit.exp = -2;
	
	//uruchomienie uart
	dk_uart_init();
	//uruchomienie probkowania
	osc.init();
	//uruchomienie LCD
	chart_init();
	
	while (1){
		//TRYB OUTPUT = LCD
		if(config.scope == LCD){
			//sprawdzenie czy w tym trybie nie przyszlo zadanie przelaczenia na tryb UART
			if(osc_uart.check_for_uart()){
				config.scope = UART;
			}
			//pomiar
			osc.meas();
			//narysowanie wykresu gdy urzadzenie
			//jest w normalnym (ciaglym) trybie wyswietlania danych pomiarowych
			if (!config.single_meas || config.snapshot){
				chart_draw();
				config.snapshot = false;
			}
		}
		//TRYB OUTPUT = UART
		else if( config.scope == UART){
			//sprawdzenie czy w tym trybie nie przyszlo zadanie przelaczenia na tryb LCD
			//obsluga zadan
			if(!osc_uart.supp_comm()){
				config.scope = LCD;
			}
		}
		
		//wyswietlanie statystyk kiedy jest to potrzebne
		if (stats_enabled)
			generate_stats();
		gui_redraw();
		gui_react_to_joy();
	}	
}
