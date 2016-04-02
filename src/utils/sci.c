#include "sci.h"
#include <math.h>

void sci_increase_n(Sci *sci, int8_t lim_man, int8_t lim_exp, uint8_t n){
	while (n--)
		sci_increase(sci, lim_man, lim_exp);
}

void sci_decrease_n(Sci *sci, int8_t lim_man, int8_t lim_exp, uint8_t n){
	while (n--)
		sci_decrease(sci, lim_man, lim_exp);
}

void sci_increase(Sci *sci, int8_t lim_man, int8_t lim_exp){
	if (sci->exp != lim_exp || sci->man != lim_man){
		++(sci->man);
		if (sci->man == 100){
			sci->man = 10;
			++(sci->exp);
		}
	}
}

void sci_decrease(Sci *sci, int8_t lim_man, int8_t lim_exp){
	if (sci->exp != lim_exp || sci->man != lim_man){
		--(sci->man);
		if (sci->man == 9){
			sci->man = 99;
			--sci->exp;
		}
	}
}

void sci_set(Sci *sci, float fl_val){
	int exp;
	float man;
	if (fl_val == 0.0){
		exp = 0;
		man = 0;
	}
	else{
		exp = log10(fl_val);
		man = fl_val - pow(10, exp);
		while (man < 10){
			man *= 10;
			--exp;
		}
	}
	sci->exp = exp;
	sci->man = man;
}

float sci_get(Sci sci){
	return sci.man * pow(10, sci.exp);
}
