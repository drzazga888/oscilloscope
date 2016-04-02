#include "LPC17xx.h"                    // Device header
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "Board_Joystick.h"             // ::Board Support:Joystick
#include "gui.h"

// czasy oczekiwania
uint16_t gui_buttons_timeouts[] = {
	GUI_BUTTON_KEY1_TIMEOUT,
	GUI_BUTTON_KEY2_TIMEOUT,
	GUI_BUTTON_INT0_TIMEOUT
};
uint16_t gui_title_timeout = GUI_TITLE_TIMEOUT;
uint16_t gui_joy_left_timeout = 0;
uint16_t gui_joy_right_timeout = 0;
uint16_t gui_joy_top_timeout = 0;
uint16_t gui_joy_bottom_timeout = 0;
uint16_t gui_joy_center_timeout = 0;

// zmienne dla joysticka

uint32_t prev_joy, joy;
uint8_t joy_left_update;
uint8_t joy_right_update;
uint8_t joy_top_update;
uint8_t joy_bottom_update;
uint8_t joy_center_update;

int16_t gui_joy_active_time = 0;

// bufory znakowe

char gui_up_buffer[40];
char gui_down_buffer[4][10];
uint8_t gui_need_redraw[8];
uint16_t gui_area_colours[8];

// przechowywanie aktywnej opcji

enum GUI_AREA gui_selected = GUI_NONE;

// blokowanie wchodzenia do gornego menu

bool gui_top_locked = false;

// funkcje

void gui_init(void){
	SysTick_Config(SystemCoreClock / 10);
	GPIO_PortClock(1);
	PIN_Configure(2, 11, 1, 2, 0);
	PIN_Configure(2, 12, 1, 2, 0);
	PIN_Configure(2, 10, 1, 2, 0);
	PIN_Configure(1, 26, 0, 0, 0);
	LPC_SC->EXTMODE |= ((1<<0) | (1<<1) | (1<<2));
	LPC_SC->EXTINT |= ((1<<0) | (1<<1) | (1<<2));
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);
	NVIC_EnableIRQ(EINT0_IRQn);
	Joystick_Initialize();
	regenerate();
}

void gui_redraw(void){
	int i;
	for (i = 0; i < 8; ++i){
		if (gui_need_redraw[i]){
			if (!stats_enabled && i < 4){
				lcd_printNText(i * MAX_X / 4, 0, gui_up_buffer + i * 10, 10, gui_area_colours[i], GUI_COLOR_BKG);
				if (i == gui_selected)
					lcd_clear(i * MAX_X / 4, 14, i * MAX_X / 4 + 80, 16, GUI_COLOR_HOVER);
			}
			else{
				lcd_printNText((i - 4) * MAX_X / 4, MAX_Y - START_Y, gui_down_buffer[i - 4], 10, gui_area_colours[i], GUI_COLOR_BKG);
				if (i == gui_selected)
					lcd_clear((i - 4) * MAX_X / 4, MAX_Y - START_Y + 14, (i - 4) * MAX_X / 4 + 80, MAX_Y - START_Y + 16, GUI_COLOR_HOVER);
			}
			gui_need_redraw[i] = false;
		}
	}
	if (stats_enabled)
		lcd_printNText(0, 0, gui_up_buffer, 40, GUI_COLOR_FONT, GUI_COLOR_BKG);
}

void gui_react_to_joy(void){
	prev_joy = joy;
	joy = Joystick_GetState();
	
	if (!(prev_joy & JOY_ROTATED_LEFT) && (joy & JOY_ROTATED_LEFT)){
		joy_left_update = 1;
		gui_joy_left_timeout = GUI_JOY_TIMEOUT;
	} else if ((joy & JOY_ROTATED_LEFT) && !gui_joy_left_timeout)
		joy_left_update = 1;
	else
		joy_left_update = 0;
	
	if (!(prev_joy & JOY_ROTATED_RIGHT) && (joy & JOY_ROTATED_RIGHT)){
		joy_right_update = 1;
		gui_joy_right_timeout = GUI_JOY_TIMEOUT;
	} else if ((joy & JOY_ROTATED_RIGHT) && !gui_joy_right_timeout)
		joy_right_update = 1;
	else
		joy_right_update = 0;
	
	if (!(prev_joy & JOY_ROTATED_UP) && (joy & JOY_ROTATED_UP)){
		joy_top_update = 1;
		gui_joy_top_timeout = GUI_JOY_TIMEOUT;
	} else if ((joy & JOY_ROTATED_UP) && !gui_joy_top_timeout)
		joy_top_update = 1;
	else
		joy_top_update = 0;
	
	if (!(prev_joy & JOY_ROTATED_DOWN) && (joy & JOY_ROTATED_DOWN)){
		joy_bottom_update = 1;
		gui_joy_bottom_timeout = GUI_JOY_TIMEOUT;
	} else if ((joy & JOY_ROTATED_DOWN) && !gui_joy_bottom_timeout)
		joy_bottom_update = 1;
	else
		joy_bottom_update = 0;
	
	if (!(prev_joy & JOYSTICK_CENTER) && (joy & JOYSTICK_CENTER)){
		joy_center_update = 1;
		gui_joy_center_timeout = GUI_JOY_TIMEOUT;
	} else if ((joy & JOYSTICK_CENTER) && !gui_joy_center_timeout)
		joy_center_update = 1;
	else
		joy_center_update = 0;
	
	if (joy){
		if (joy_left_update)
			gui_joy_left();
		else if (joy_right_update)
			gui_joy_right();
		else if (joy_top_update)
			gui_joy_up();
		else if (joy_bottom_update)
			gui_joy_down();
		else if (joy_center_update)
			gui_joy_center();
	}
}

void gui_sci_increase(Sci *sci, int8_t lim_man, int8_t lim_exp){
	int saved = gui_joy_active_time;
	sci_increase_n(sci, lim_man, lim_exp, saved + 1);
	gui_joy_active_time = 0;
}

void gui_sci_decrease(Sci *sci, int8_t lim_man, int8_t lim_exp){
	int saved = gui_joy_active_time;
	sci_decrease_n(sci, lim_man, lim_exp, saved + 1);
	gui_joy_active_time = 0;
}

// obsluga przerwan

void SysTick_Handler(void){
	if (gui_buttons_timeouts[GUI_KEY1])
		gui_buttons_timeouts[GUI_KEY1] -= GUI_IRQ_TIME_DECREMENT;
	if (gui_buttons_timeouts[GUI_KEY2])
		gui_buttons_timeouts[GUI_KEY2] -= GUI_IRQ_TIME_DECREMENT;
	if (gui_buttons_timeouts[GUI_INT0])
		gui_buttons_timeouts[GUI_INT0] -= GUI_IRQ_TIME_DECREMENT;
	if (gui_joy_left_timeout)
		gui_joy_left_timeout -= GUI_IRQ_TIME_DECREMENT;
	else if (joy & JOY_ROTATED_LEFT)
		++gui_joy_active_time;
	if (gui_joy_right_timeout)
		gui_joy_right_timeout -= GUI_IRQ_TIME_DECREMENT;
	else if (joy & JOY_ROTATED_RIGHT)
		++gui_joy_active_time;
	if (gui_joy_top_timeout)
		gui_joy_top_timeout -= GUI_IRQ_TIME_DECREMENT;
	else if (joy & JOY_ROTATED_UP)
		++gui_joy_active_time;
	if (gui_joy_bottom_timeout)
		gui_joy_bottom_timeout -= GUI_IRQ_TIME_DECREMENT;
	else if (joy & JOY_ROTATED_DOWN)
		++gui_joy_active_time;
	if (gui_joy_center_timeout)
		gui_joy_center_timeout -= GUI_IRQ_TIME_DECREMENT;
}

void EINT1_IRQHandler(void){
	if (!gui_buttons_timeouts[GUI_KEY1]){
		gui_buttons_timeouts[GUI_KEY1] = GUI_BUTTON_KEY1_TIMEOUT;
		gui_key1();
	}
	LPC_SC->EXTINT |= (1 << 1);
}

void EINT2_IRQHandler(void){
	if (!gui_buttons_timeouts[GUI_KEY2]){
		gui_buttons_timeouts[GUI_KEY2] = GUI_BUTTON_KEY2_TIMEOUT;
		gui_key2();
	}
	LPC_SC->EXTINT |= (1 << 2);
}

void EINT0_IRQHandler(void){
	if (!gui_buttons_timeouts[GUI_INT0]){
		gui_buttons_timeouts[GUI_INT0] = GUI_BUTTON_INT0_TIMEOUT;
		gui_int0();
	}
	LPC_SC->EXTINT |= (1 << 0);
}
