#include <string.h>
#include "LPC17xx.h"                    // Device header
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "uart.h"
#include "../adc/ad_prober.h"
#include "../osc/config.h"
#include "../osc/osc.h"

char RXD_buffer[17];
uint8_t RXD_iter = 0;
uint32_t comm_counter=0;


void dk_uart_init(void){
	LPC_SC->PCONP |= (1 << 24); //podl. zrodla zegarowego
	LPC_SC->PCLKSEL1 |= (1<<16); //div cclk/1
	LPC_UART2->LCR |= (1 << 7); //DLAB access
	LPC_UART2->LCR |= 3; //ramka 8-bit
	
	//ustawienie optymalnego preskalera
	LPC_UART2->DLL = 31; 
	LPC_UART2->DLM = 0;
	LPC_UART2->FDR = (3 | (4<<4));
	
	LPC_UART2->FCR = 7; //wl FIFO i reset
	PIN_Configure(0, 10, 1, 2, 0); //podl pinow
	PIN_Configure(0, 11, 1, 2, 0);
	LPC_UART2->LCR &= ~(1 << 7); //DLAB access
	
	//przerwanie odbioru danych
	LPC_UART2->IER |= (1<<0); 
	NVIC_EnableIRQ(UART2_IRQn);
}


void dk_transmit(){
	uint16_t j;
	//pomiar
	osc.meas();

	//wysylanie mlodszych potem starszych bitow 
	for(j=0; j<640; ++j){
		while (!(LPC_UART2->LSR & (1 << 5)))
			__NOP();
		LPC_UART2->THR = (config.channel[CH1].src[j])&0xFF;
		while (!(LPC_UART2->LSR & (1 << 5)))
			__NOP();
		LPC_UART2->THR = (config.channel[CH1].src[j])>>8;
	}
	
	for(j=0; j<640; ++j){
		while (!(LPC_UART2->LSR & (1 << 5)))
			__NOP();
		LPC_UART2->THR = (config.channel[CH2].src[j])&0xFF;
		while (!(LPC_UART2->LSR & (1 << 5)))
			__NOP();
		LPC_UART2->THR = (config.channel[CH2].src[j])>>8;
	}
	
	printf("\n\r");
}

bool uart_exec_comm(void){
	
	if(config.over) return true;
	if(!strcmp(RXD_buffer, "GET_A_MEAS")){
			dk_transmit();
			printf("%u\n\r", comm_counter++);
		}		
		else if(!strcmp(RXD_buffer, "LOOP_BACK")){
			printf("LOOP_BACK\n\r");
			printf("%u\n\r", comm_counter++);
		}
		else if(!strcmp(RXD_buffer, "ID_COMM_RESET")){
			printf("ID_COMM_RESETED\n\r");
			comm_counter = 0;
			printf("%u\n\r", comm_counter++);
		}
		else if(!strcmp(RXD_buffer, "LCD_ENABLE")){
			printf("LCD_ENABLED\n\r");
			printf("%u\n\r", comm_counter++);
			config.over = true;
			return false;
		}
		
		config.over = true;
		return true;
}

//funkcja obslugi odbioru ramek
void UART2_IRQHandler(void){
	unsigned char sign = LPC_UART2->RBR;
	
	if(config.over && sign!='\n' && RXD_iter<16){
		RXD_buffer[RXD_iter++] = sign;
		RXD_buffer[RXD_iter] = '\0';
	}
	else if(sign=='\n'){
			RXD_iter = 0;
			config.over = false;
	}
	//else obsluga przepelnionego FIFO
	//TODO czyszczenie buforu na timeout odbioru
}

//funkcja testujaca zewnetrzen zadanie przelaczenia w tryb UART (z trybu LCD)
bool uart_check_sw(void){
	if(config.over) return false;
	else if(!strcmp(RXD_buffer, "UART_ENABLE")){
		printf("UART_ENABLED\n\r");
		comm_counter = 0;
		printf("%u\n\r", comm_counter++);
		config.over = true;
		return true;
	}
	else{
		config.over = true;
		return false;
	}
}
