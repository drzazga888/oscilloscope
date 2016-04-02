#include <math.h>
#include <string.h>
#include "LPC17xx.h"                    // Device header
#include "../osc/config.h"
#include "../osc/osc.h"
#include "chart.h"
#include "gui.h"

// globals
uint16_t saved[2][MAX_X];
uint8_t is_drawed[2];
uint8_t is_active[2];
float mul_saved;
int32_t off_saved[2];
uint16_t temp[2];
int32_t chart_last_trig, chart_trig;
enum CHANNEL last_trig_channel;

// kolory
const uint16_t channel_color[] = {
	GUI_COLOR_ACCENT1,
	GUI_COLOR_ACCENT2
};

// functions
void chart_init(void){
	lcd_init();
	lcd_pattern = chart_mesh;
	lcd_clear_all(0);
	lcd_pattern = 0;
	chart_set_trig();
	gui_init();
}

void chart_set_trig(void){
	float mul_v = 40.0 / sci_get(config.dv);
	chart_trig = MAX_Y - 1 - mul_v * sci_get(config.trig_level) + config.channel[config.trigger_channel].y - START_Y;
}

void chart_draw_trig(void){
	if (last_trig_channel != NO_CHANNEL){
		lcd_pattern = chart_mesh;
		lcd_drawPoint(0, chart_last_trig, 0);
		lcd_drawPoint(1, chart_last_trig, 0);
		lcd_drawPoint(2, chart_last_trig, 0);
		lcd_drawPoint(0, chart_last_trig - 1, 0);
		lcd_drawPoint(0, chart_last_trig + 1, 0);
		lcd_drawPoint(0, chart_last_trig - 2, 0);
		lcd_drawPoint(0, chart_last_trig + 2, 0);
		lcd_drawPoint(1, chart_last_trig - 1, 0);
		lcd_drawPoint(1, chart_last_trig + 1, 0);
		lcd_pattern = 0;
	}
	if (config.trigger_channel != NO_CHANNEL){
		lcd_drawPoint(0, chart_trig, channel_color[config.trigger_channel]);
		lcd_drawPoint(1, chart_trig, channel_color[config.trigger_channel]);
		lcd_drawPoint(2, chart_trig, channel_color[config.trigger_channel]);
		lcd_drawPoint(0, chart_trig - 1, channel_color[config.trigger_channel]);
		lcd_drawPoint(0, chart_trig + 1, channel_color[config.trigger_channel]);
		lcd_drawPoint(0, chart_trig - 2, channel_color[config.trigger_channel]);
		lcd_drawPoint(0, chart_trig + 2, channel_color[config.trigger_channel]);
		lcd_drawPoint(1, chart_trig - 1, channel_color[config.trigger_channel]);
		lcd_drawPoint(1, chart_trig + 1, channel_color[config.trigger_channel]);
	}
	chart_last_trig = chart_trig;
	last_trig_channel = config.trigger_channel;
}

uint16_t chart_mesh(uint16_t x, uint16_t y){
	if (y < START_Y || y >= MAX_Y - START_Y)
		return GUI_COLOR_BKG;
	else if (x % 40 == 0 || (y + START_Y) % 40 == 0)
		return RGB(60, 60, 60);
	else if (x % 8 == 0 || (y + START_Y) % 8 == 0)
		return chart_sub_mesh(x, y);
	return RGB(0, 0, 0);
}

uint16_t chart_sub_mesh(uint16_t x, uint16_t y){
	if (x % 2 == 0 && (y + START_Y) % 2 == 0)
		return RGB(20, 20, 20);
	return RGB(0, 0, 0);
}
	
void chart_draw(void){
	int i, channel;
	float mul_v = 3.3 * 40.0 / 4096.0 / sci_get(config.dv);
	uint16_t trig = MAX_X / 2;

	if (config.trigger_channel != NO_CHANNEL)
		trig = osc.digital_trig(config.channel[config.trigger_channel].src, (uint16_t)(sci_get(config.trig_level)/3.3*4096), config.trigger_mode);
	
	is_active[CH1] = config.channel[CH1].is_active;
	is_active[CH2] = config.channel[CH2].is_active;
	lcd_setClippingRegion(0, START_Y, MAX_X, MAX_Y - START_Y);
	for (i = 0; i < MAX_X; ++i){
		for (channel = 0; channel < 2; ++channel){
			if (!is_active[channel] && is_drawed[channel]){
				if (i != 0){
					lcd_pattern = chart_mesh;
					lcd_drawLine(
						i - 1,
						MAX_Y - 1 - mul_saved * saved[channel][i - 1] + off_saved[channel] - START_Y,
						i,
						MAX_Y - 1 - mul_saved * saved[channel][i] + off_saved[channel] - START_Y,
						0
					);
					lcd_pattern = 0;
				}
			} else if (is_drawed[channel]){
				if (i != 0){
					lcd_pattern = chart_mesh;
					lcd_drawLine(
						i - 1,
						MAX_Y - 1 - mul_saved * temp[channel] + off_saved[channel] - START_Y,
						i,
						MAX_Y - 1 - mul_saved * saved[channel][i]  + off_saved[channel] - START_Y,
						0
					);
					lcd_pattern = 0;
				}
			}
			if (is_active[channel]){
				temp[channel] = saved[channel][i];
				if (i + trig + config.channel[channel].x - (MAX_X / 2) < 0){
					saved[channel][i] = 0;
					continue;
				}
				if (i + trig + config.channel[channel].x - (MAX_X / 2) >= config.n){
					saved[channel][i] = 0;
					continue;
				}
				saved[channel][i] = config.channel[channel].src[i + trig + config.channel[channel].x - (MAX_X / 2)];
				if (i != 0){
					lcd_drawLine(
						i - 1,
						MAX_Y - 1 - mul_v * saved[channel][i - 1] + config.channel[channel].y - START_Y,
						i,
						MAX_Y - 1 - mul_v * saved[channel][i] + config.channel[channel].y - START_Y,
						channel_color[channel]
					);
				}
			}
		}
	}
	mul_saved = mul_v;
	for (channel = 0; channel < 2; ++channel){
		is_drawed[channel] = is_active[channel];
		off_saved[channel] = config.channel[channel].y;
	}
	chart_draw_trig();
	lcd_unsetClippingRegion();
}
