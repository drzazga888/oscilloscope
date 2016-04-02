#include "gui_controller.h"
#include "../osc/osc.h"

enum GUI_OPTION gui_option_selected = GUI_NO_OPTION;

void gui_int0(void){
	switch (gui_selected){
	case GUI_UP3:
		config.channel[CH1].is_active = !config.channel[CH1].is_active;
		generate_ch1();
		break;
	case GUI_UP4:
		config.channel[CH2].is_active = !config.channel[CH2].is_active;
		generate_ch2();
		break;
	case GUI_DOWN3:
		switch (gui_option_selected){
			case GUI_SINGLE_MEAS:
				if (config.single_meas)
					config.snapshot = true;
				break;
			default:
				break;
		}
		break;
	default:
		break;
	}
}

void gui_key1(void){
	if (gui_selected != GUI_NONE)
		gui_need_redraw[gui_selected] = true;
	if (gui_selected == (gui_top_locked ? 4 : 0))
		gui_selected = (enum GUI_AREA)8;
	else
		--gui_selected;
	if (gui_selected != GUI_NONE)
		gui_need_redraw[gui_selected] = true;
	gui_joy_active_time = 0;
}

void gui_key2(void){
	if (gui_selected != GUI_NONE)
		gui_need_redraw[gui_selected] = true;
	++gui_selected;
	if (gui_selected > 8)
		gui_selected = (enum GUI_AREA)(gui_top_locked ? 4 : 0);
	if (gui_selected != GUI_NONE)
		gui_need_redraw[gui_selected] = true;
	gui_joy_active_time = 0;
}

void gui_joy_up(void){
	switch (gui_selected){
	case GUI_UP1:
		gui_sci_increase(&config.dt, MAX_DT_MAN, MAX_DT_EXP);
		generate_dt();
		break;
	case GUI_UP2:
		gui_sci_increase(&config.dv, MAX_DV_MAN, MAX_DV_EXP);
		chart_set_trig();
		generate_dv();
		break;
	case GUI_UP3:
		config.channel[CH1].y -= DX;
		chart_set_trig();
		break;
	case GUI_UP4:
		config.channel[CH2].y -= DX;
		chart_set_trig();
		break;
	case GUI_DOWN1:
		if (gui_option_selected == 0)
			gui_option_selected = (enum GUI_OPTION)(GUI_OPTION_SIZE - 1);
		else
			--gui_option_selected;
		generate_options();
		break;
	case GUI_DOWN2:
		switch (gui_option_selected){
		case GUI_OUTPUT_MODE:
			if (config.scope == LCD)
				config.scope = UART;
			else
				config.scope = LCD;
			break;
		case GUI_SINGLE_MEAS:
			config.single_meas = !config.single_meas;
			break;
		case GUI_TRIGGER:
			if (config.trigger_channel == 0)
				config.trigger_channel = (enum CHANNEL)2;
			else
				--config.trigger_channel;
			break;
		case GUI_MEDIAN_FILTER_CH1:
			config.channel[CH1].median_on = !config.channel[CH1].median_on;
			break;
		case GUI_MEDIAN_FILTER_CH2:
			config.channel[CH2].median_on = !config.channel[CH2].median_on;
			break;
		case GUI_STATS:
			stats_enabled = !stats_enabled;
			gui_top_locked = !gui_top_locked;
			if (!stats_enabled)
				regenerate();
			break;
		default:
			break;
		}
		generate_options();
		break;
	case GUI_DOWN3:
		switch (gui_option_selected){
		case GUI_TRIGGER:
			gui_sci_increase(&config.trig_level, MAX_TRIG_LVL_MAN, MAX_TRIG_LVL_EXP);
			chart_set_trig();
			break;
		case GUI_MEDIAN_FILTER_CH1:
			gui_sci_increase(&config.channel[CH1].anomalies_height_limit, MAX_MED_FILT_MAN, MAX_MED_FILT_EXP);
			break;
		case GUI_MEDIAN_FILTER_CH2:
			gui_sci_increase(&config.channel[CH2].anomalies_height_limit, MAX_MED_FILT_MAN, MAX_MED_FILT_EXP);
			break;
		case GUI_STATS:
			if (stats_target == 0)
				stats_target = (enum STATS_TARGET)3;
			else
				--stats_target;
			break;
		default:
			break;
		}
		generate_options();
		break;
	case GUI_DOWN4:
		switch (gui_option_selected){
		case GUI_TRIGGER:
			if (config.trigger_mode == RISING_EDGE)
				config.trigger_mode = FALLING_EDGE;
			else
				config.trigger_mode = RISING_EDGE;
			break;
		case GUI_STATS:
			if (stats_type == STATS_FREQ)
				stats_type = STATS_AMP;
			else
				stats_type = STATS_FREQ;
			break;
		default:
			break;
		}
		generate_options();
		break;
	default:
		break;
	}
}

void gui_joy_down(void){
	switch (gui_selected){
	case GUI_UP1:
		gui_sci_decrease(&config.dt, MIN_DT_MAN, MIN_DT_EXP);
		generate_dt();
		break;
	case GUI_UP2:
		gui_sci_decrease(&config.dv, MIN_DV_MAN, MIN_DV_EXP);
		chart_set_trig();
		generate_dv();
		break;
	case GUI_UP3:
		config.channel[CH1].y += DX;
		chart_set_trig();
		break;
	case GUI_UP4:
		config.channel[CH2].y += DX;
		chart_set_trig();
		break;
	case GUI_DOWN1:
		++gui_option_selected;
		if (gui_option_selected >= GUI_OPTION_SIZE)
			gui_option_selected = (enum GUI_OPTION)0;
		generate_options();
		break;
	case GUI_DOWN2:
		switch (gui_option_selected){
		case GUI_OUTPUT_MODE:
			if (config.scope == LCD)
				config.scope = UART;
			else
				config.scope = LCD;
			break;
		case GUI_SINGLE_MEAS:
			config.single_meas = !config.single_meas;
			break;
		case GUI_TRIGGER:
			++config.trigger_channel;
			if (config.trigger_channel > 2)
				config.trigger_channel = (enum CHANNEL)0;
			break;
		case GUI_MEDIAN_FILTER_CH1:
			config.channel[CH1].median_on = !config.channel[CH1].median_on;
			break;
		case GUI_MEDIAN_FILTER_CH2:
			config.channel[CH2].median_on = !config.channel[CH2].median_on;
			break;
		case GUI_STATS:
			stats_enabled = !stats_enabled;
			gui_top_locked = !gui_top_locked;
			if (!stats_enabled)
				regenerate();
			break;
		default:
			break;
		}
		generate_options();
		break;
	case GUI_DOWN3:
		switch (gui_option_selected){
		case GUI_TRIGGER:
			gui_sci_decrease(&config.trig_level, MIN_TRIG_LVL_MAN, MIN_TRIG_LVL_EXP);
			chart_set_trig();
			break;
		case GUI_MEDIAN_FILTER_CH1:
			gui_sci_decrease(&config.channel[CH1].anomalies_height_limit, MIN_MED_FILT_MAN, MIN_MED_FILT_EXP);
			break;
		case GUI_MEDIAN_FILTER_CH2:
			gui_sci_decrease(&config.channel[CH2].anomalies_height_limit, MIN_MED_FILT_MAN, MIN_MED_FILT_EXP);
			break;
		case GUI_STATS:
			++stats_target;
			if (stats_target >= 4)
				stats_target = (enum STATS_TARGET)0;
			break;
		default:
			break;
		}
		generate_options();
		break;
	case GUI_DOWN4:
		switch (gui_option_selected){
		case GUI_TRIGGER:
			if (config.trigger_mode == RISING_EDGE)
				config.trigger_mode = FALLING_EDGE;
			else
				config.trigger_mode = RISING_EDGE;
			break;
		case GUI_STATS:
			if (stats_type == STATS_FREQ)
				stats_type = STATS_AMP;
			else
				stats_type = STATS_FREQ;
			break;
		default:
			break;
		}
		generate_options();
		break;
	default:
		break;
	}
}

void gui_joy_left(void){
	switch (gui_selected){
	case GUI_UP3:
		config.channel[CH1].x += DX;
		break;
	case GUI_UP4:
		config.channel[CH2].x += DX;
		break;
	default:
		break;
	}
}

void gui_joy_right(void){
	switch (gui_selected){
	case GUI_UP3:
		config.channel[CH1].x -= DX;
		break;
	case GUI_UP4:
		config.channel[CH2].x -= DX;
		break;
	default:
		break;
	}
}

void gui_joy_center(void){
	
}

void regenerate(void){
	generate_dt();
	generate_dv();
	generate_ch1();
	generate_ch2();
	generate_options();
}

void generate_dt(void){
	int pos = 3;
	strncpy(gui_up_buffer, "dt:", 3);
	pos += scitoa(config.dt, gui_up_buffer + 3, 2);
	strncpy(gui_up_buffer + (pos++), "s", 1);
	while (pos < 10)
		gui_up_buffer[pos++] = ' ';
	gui_area_colours[GUI_UP1] = White;
	gui_need_redraw[GUI_UP1] = true;
}

void generate_dv(void){
	int pos = 3;
	strncpy(gui_up_buffer + 10, "dv:", 3);
	pos += scitoa(config.dv, gui_up_buffer + 13, 2);
	strncpy(gui_up_buffer + 10 + (pos++), "V", 1);
	while (pos < 10)
		strncpy(gui_up_buffer + 10 + (pos++), " ", 1);
	gui_area_colours[GUI_UP2] = White;
	gui_need_redraw[GUI_UP2] = true;
}

void generate_ch1(void){
	strncpy(gui_up_buffer + 20, "channel 1 ", 10);
	gui_area_colours[GUI_UP3] = config.channel[CH1].is_active ? GUI_COLOR_ACCENT1 : GUI_COLOR_DISABLED;
	gui_need_redraw[GUI_UP3] = true;
}

void generate_ch2(void){
	strncpy(gui_up_buffer + 30, "channel 2 ", 10);
	gui_area_colours[GUI_UP4] = config.channel[CH2].is_active ? GUI_COLOR_ACCENT2 : GUI_COLOR_DISABLED;
	gui_need_redraw[GUI_UP4] = true;
}

void generate_options(void){
	int pos;
	gui_area_colours[GUI_DOWN1] = White;
	gui_area_colours[GUI_DOWN2] = White;
	gui_area_colours[GUI_DOWN3] = White;
	gui_area_colours[GUI_DOWN4] = White;
	gui_need_redraw[GUI_DOWN1] = true;
	gui_need_redraw[GUI_DOWN2] = true;
	gui_need_redraw[GUI_DOWN3] = true;
	gui_need_redraw[GUI_DOWN4] = true;
	switch (gui_option_selected)
	{
	case GUI_NO_OPTION:
		strncpy(gui_down_buffer[0], "more...   ", 10);
		strncpy(gui_down_buffer[1], "          ", 10);
		strncpy(gui_down_buffer[2], "          ", 10);
		strncpy(gui_down_buffer[3], "          ", 10);
		break;
	case GUI_OUTPUT_MODE:
		strncpy(gui_down_buffer[0], "output    ", 10);
		if (config.scope == LCD)
			strncpy(gui_down_buffer[1], " LCD      ", 10);
		else
			strncpy(gui_down_buffer[1], " UART     ", 10);
		strncpy(gui_down_buffer[2], "          ", 10);
		strncpy(gui_down_buffer[3], "          ", 10);
		break;
	case GUI_SINGLE_MEAS:
		strncpy(gui_down_buffer[0], "call meas ", 10);
		if (config.single_meas){
			strncpy(gui_down_buffer[1], " ON       ", 10);
			gui_area_colours[GUI_DOWN2] = Green;
		}
		else{
			strncpy(gui_down_buffer[1], " OFF      ", 10);
			gui_area_colours[GUI_DOWN2] = Red;
		}
		if (!config.single_meas){
			gui_area_colours[GUI_DOWN3] = GUI_COLOR_DISABLED;
		}
		strncpy(gui_down_buffer[2], "snapshot  ", 10);
		strncpy(gui_down_buffer[3], "          ", 10);
		break;
	case GUI_TRIGGER:
		strncpy(gui_down_buffer[0], "trigger   ", 10);
		switch (config.trigger_channel){
		case CH1:
			strncpy(gui_down_buffer[1], " CH1      ", 10);
			break;
		case CH2:
			strncpy(gui_down_buffer[1], " CH2      ", 10);
			break;
		case NO_CHANNEL:
			strncpy(gui_down_buffer[1], " none     ", 10);
			break;
		}
		strncpy(gui_down_buffer[2], "lv ", 3);
		pos = 3 + scitoa(config.trig_level, gui_down_buffer[2] + 3, 2);
		strncpy(gui_down_buffer[2] + (pos++), "V", 1);
		while (pos < 10)
			gui_down_buffer[2][pos++] = ' ';
		if (config.trigger_mode == FALLING_EDGE)
			strncpy(gui_down_buffer[3], "   falling", 10);
		else
			strncpy(gui_down_buffer[3], "    rising", 10);
		break;
	case GUI_MEDIAN_FILTER_CH1:
		strncpy(gui_down_buffer[0], "med f CH1 ", 10);
		if (config.channel[CH1].median_on){
			strncpy(gui_down_buffer[1], " ON       ", 10);
			gui_area_colours[GUI_DOWN2] = Green;
		}
		else{
			strncpy(gui_down_buffer[1], " OFF      ", 10);
			gui_area_colours[GUI_DOWN2] = Red;
		}
		strncpy(gui_down_buffer[2], "lo ", 3);
		pos = 3 + scitoa(config.channel[CH1].anomalies_height_limit, gui_down_buffer[2] + 3, 2);
		strncpy(gui_down_buffer[2] + (pos++), "V", 1);
		while (pos < 10)
			gui_down_buffer[2][pos++] = ' ';
		strncpy(gui_down_buffer[3], "  degree 1", 10);
		break;
	case GUI_MEDIAN_FILTER_CH2:
		strncpy(gui_down_buffer[0], "med f CH2 ", 10);
		if (config.channel[CH2].median_on){
			strncpy(gui_down_buffer[1], " ON       ", 10);
			gui_area_colours[GUI_DOWN2] = Green;
		}
		else{
			strncpy(gui_down_buffer[1], " OFF      ", 10);
			gui_area_colours[GUI_DOWN2] = Red;
		}
		strncpy(gui_down_buffer[2], "lo ", 3);
		pos = 3 + scitoa(config.channel[CH2].anomalies_height_limit, gui_down_buffer[2] + 3, 2);
		strncpy(gui_down_buffer[2] + (pos++), "V", 1);
		while (pos < 10)
			gui_down_buffer[2][pos++] = ' ';
		strncpy(gui_down_buffer[3], "  degree 1", 10);
		break;
	case GUI_STATS:
		strncpy(gui_down_buffer[0], "statistics", 10);
		if (stats_enabled){
			strncpy(gui_down_buffer[1], " ON       ", 10);
			gui_area_colours[GUI_DOWN2] = Green;
		}
		else{
			strncpy(gui_down_buffer[1], " OFF      ", 10);
			gui_area_colours[GUI_DOWN2] = Red;
		}
		switch (stats_target){
		case STATS_CH1:
			strncpy(gui_down_buffer[2], " CH1      ", 10);
			break;
		case STATS_CH2:
			strncpy(gui_down_buffer[2], " CH2      ", 10);
			break;
		case STATS_MAX_MIN:
			strncpy(gui_down_buffer[2], " min/max  ", 10);
			break;
		case STATS_AVR_DEV:
			strncpy(gui_down_buffer[2], " avr/dev  ", 10);
			break;
		}
		if (stats_type == STATS_FREQ){
			strncpy(gui_down_buffer[3], " frequency", 10);
		}
		else{
			strncpy(gui_down_buffer[3], " amplitude", 10);
		}
		break;
	}
}

void generate_stats(void){

	int pos;
	struct stats_t *curr_stat;

	if (stats_target == STATS_CH1)
		curr_stat = &stats1;
	else if (stats_target == STATS_CH2)
		curr_stat = &stats2;

	osc.statistics();

	switch (stats_target){

	case STATS_CH1:
	case STATS_CH2:
		pos = ftoa_units(stats_type == STATS_FREQ ? curr_stat->avr_f : curr_stat->avr_amp, gui_up_buffer, 2);
		while (pos < 7)
			gui_up_buffer[pos++] = ' ';
		strncpy(gui_up_buffer + pos, "+- ", 3);
		pos += 3;
		pos += ftoa_units(stats_type == STATS_FREQ ? curr_stat->dev_f : curr_stat->dev_amp, gui_up_buffer + pos, 2);
		strncpy(gui_up_buffer + pos, stats_type == STATS_FREQ ? "Hz" : "V ", 2);
		pos += 2;
		while (pos < 19)
			gui_up_buffer[pos++] = ' ';
		strncpy(gui_up_buffer + pos, "H:", 2);
		pos += 2;
		pos += ftoa_units(stats_type == STATS_FREQ ? curr_stat->max_f : curr_stat->max_amp, gui_up_buffer + pos, 2);
		strncpy(gui_up_buffer + pos, stats_type == STATS_FREQ ? "Hz" : "V ", 2);
		pos += 2;
		while (pos < 30)
			gui_up_buffer[pos++] = ' ';
		strncpy(gui_up_buffer + pos, "L:", 2);
		pos += 2;
		pos += ftoa_units(stats_type == STATS_FREQ ? curr_stat->min_f : curr_stat->min_amp, gui_up_buffer + pos, 2);
		strncpy(gui_up_buffer + pos, stats_type == STATS_FREQ ? "Hz" : "V ", 2);
		pos += 2;
		while (pos < 40)
			gui_up_buffer[pos++] = ' ';
		break;

	case STATS_MAX_MIN:
		strncpy(gui_up_buffer, "1 L:", 4);
		pos = 4;
		pos += ftoa_units(stats_type == STATS_FREQ ? stats1.min_f : stats1.min_amp, gui_up_buffer + pos, 2);
		while (pos < 11)
			gui_up_buffer[pos++] = ' ';
		strncpy(gui_up_buffer + pos, "H:", 2);
		pos += 2;
		pos += ftoa_units(stats_type == STATS_FREQ ? stats1.max_f : stats1.max_amp, gui_up_buffer + pos, 2);
		while (pos < 20)
			gui_up_buffer[pos++] = ' ';
		strncpy(gui_up_buffer + pos, "2 L:", 4);
		pos += 4;
		pos += ftoa_units(stats_type == STATS_FREQ ? stats2.min_f : stats2.min_amp, gui_up_buffer + pos, 2);
		while (pos < 31)
			gui_up_buffer[pos++] = ' ';
		strncpy(gui_up_buffer + pos, "H:", 2);
		pos += 2;
		pos += ftoa_units(stats_type == STATS_FREQ ? stats2.max_f : stats2.max_amp, gui_up_buffer + pos, 2);
		while (pos < 40)
			gui_up_buffer[pos++] = ' ';
		break;

	case STATS_AVR_DEV:
		strncpy(gui_up_buffer, "1 ", 2);
		pos = 2;
		pos += ftoa_units(stats_type == STATS_FREQ ? stats1.avr_f : stats1.avr_amp, gui_up_buffer + pos, 2);
		while (pos < 9)
			gui_up_buffer[pos++] = ' ';
		strncpy(gui_up_buffer + pos, "+-", 2);
		pos += 2;
		pos += ftoa_units(stats_type == STATS_FREQ ? stats1.dev_f : stats1.dev_amp, gui_up_buffer + pos, 2);
		strncpy(gui_up_buffer + pos, stats_type == STATS_FREQ ? "Hz" : "V ", 2);
		pos += 2;
		while (pos < 20)
			gui_up_buffer[pos++] = ' ';
		strncpy(gui_up_buffer + pos, "2 ", 3);
		pos += 2;
		pos += ftoa_units(stats_type == STATS_FREQ ? stats2.avr_f : stats2.avr_amp, gui_up_buffer + pos, 2);
		while (pos < 29)
			gui_up_buffer[pos++] = ' ';
		strncpy(gui_up_buffer + pos, "+-", 2);
		pos += 2;
		pos += ftoa_units(stats_type == STATS_FREQ ? stats2.dev_f : stats2.dev_amp, gui_up_buffer + pos, 2);
		strncpy(gui_up_buffer + pos, stats_type == STATS_FREQ ? "Hz" : "V ", 2);
		pos += 2;
		while (pos < 40)
			gui_up_buffer[pos++] = ' ';
		break;

	}

}
