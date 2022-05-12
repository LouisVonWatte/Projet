#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <main.h>
#include <leds.h>
#include <selector.h>


int get_selector_color(void){
	int color = 0;
	while(1){

		static int s_rouge = 0;
		static int s_vert = 0;
		static int s_bleu = 0;
		int s = get_selector();

		if(s == 4){
			set_rgb_led(0, 15, 0, 0);
			set_rgb_led(3, 15, 0, 0);
			s_rouge++;
			s_vert = 0;
			s_bleu = 0;
		}
		if(s == 8){
			set_rgb_led(0, 0, 15, 0);
			set_rgb_led(3, 0, 15, 0);
			s_rouge = 0;
			s_vert++;
			s_bleu = 0;
		}
		if(s == 12){
			set_rgb_led(0, 0, 0, 15);
			set_rgb_led(3, 0, 0, 15);
			s_rouge = 0;
			s_vert = 0;
			s_bleu++;
		}
		if(s != 4 && s!= 8 && s!= 12){
			clear_leds();
			s_rouge = 0;
			s_vert = 0;
			s_bleu = 0;
		}

		if(s_rouge >= 1000000){
			set_rgb_led(0, 15, 0, 0);
			set_rgb_led(1, 15, 0, 0);
			set_rgb_led(2, 15, 0, 0);
			set_rgb_led(3, 15, 0, 0);
			color = RED;
			break;
		}
		if(s_vert >= 1000000){
			set_rgb_led(0, 0, 15, 0);
			set_rgb_led(1, 0, 15, 0);
			set_rgb_led(2, 0, 15, 0);
			set_rgb_led(3, 0, 15, 0);
			color = GREEN;
			break;
		}
		if(s_bleu >= 1000000){
			set_rgb_led(0, 0, 0, 15);
			set_rgb_led(1, 0, 0, 15);
			set_rgb_led(2, 0, 0, 15);
			set_rgb_led(3, 0, 0, 15);
			color = BLUE;
			break;
		}
	}
	return color;
}
