#include "retarget.h"
#include "LPC17xx.h"                    // Device header

FILE __stdout;
FILE __stdin;

void _sys_exit(int ret){
	while(1);
}

int fputc ( int character, FILE * stream ){
	while (!(LPC_UART2->LSR & (1 << 5)))
			__NOP();
	LPC_UART2->THR = (char)character;
	return 0;
}

int fgetc ( FILE * stream ){
	while (!(LPC_UART2->LSR & 1))
			__NOP();
	return (int)LPC_UART2->RBR;
}

int __backspace(FILE *stream){
	return 0;
}
