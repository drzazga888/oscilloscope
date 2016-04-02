#ifndef __RETARGET_H
#define __RETARGET_H

#include <stdio.h>
#include <rt_sys.h>

#pragma import(__use_no_semihosting_swi)

struct __FILE {
	int handle;
};

int fputc ( int character, FILE * stream );

int fgetc ( FILE * stream );

int __backspace(FILE *stream);

#endif
