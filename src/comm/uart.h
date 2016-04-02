#ifndef __UART_H
#define __UART_H

#include "retarget.h"

//inicjalizacja uart
/**
output: UART2 COM2
baud: 115200
FIFO: yes
error: 0.006400%
FR=1.75, DL=31, PCKL*=1
**/
void dk_uart_init(void);

//funkcja testujaca zewnetrzen zadanie przelaczenia w tryb UART (z trybu LCD)
bool uart_check_sw(void);

//funkcja obslugi zadan
bool uart_exec_comm(void);

static struct{
	bool(*check_for_uart)(void);
	bool(*supp_comm)(void);	
}osc_uart = {uart_check_sw, uart_exec_comm};

//transmisja danych pomiarowych
//dwie ramki na probke, kodowanie uint16_t
void dk_transmit(void);

#endif
