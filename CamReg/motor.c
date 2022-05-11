#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <main.h>
#include <motors.h>
#include <motor.h>
#include <leds.h>
#include <sensors/proximity.h>
#include <process_image.h>

static int s = 400;

void stop(void){
	right_motor_set_speed(0);
	left_motor_set_speed(0);
	set_body_led(1);
	set_front_led(0);
}

void check_turn (int color){

//		int max_val = 0;

        if(get_prox(2) >= 100 && get_prox(5) >= 100){								//forwards if both walls
        	move(s, FORWARD, 0);
        } else if(get_prox(5) < 70 && get_prox(0) > 70 && get_prox(7) > 70){		//left if missing left wall
			while(get_prox(7) < 200){
	        	move(s, FORWARD, 0);
			}
			move(s, LEFT, 323);
//			while(get_prox(2) > max_val){
//				max_val = get_prox(2);
//				move(s, LEFT, 20);
//			}
			move(s, FORWARD, 0);
			keep(4);
        } else if(get_prox(2) < 70 && get_prox(0) > 70 && get_prox(7) > 70){		//right if missing right wall
			while(get_prox(7) < get_prox(5)){
	        	move(s, FORWARD, 0);
			}
			move(s, RIGHT, 323);
//			while(get_prox(4) > get_prox(3)){
//				move(50, RIGHT, 0);
//			}
//
//			while(get_prox(3) > get_prox(4)){
//				move(50, LEFT, 0);
//			}
			move(s, FORWARD, 0);
			keep(4);
        } else if(get_prox(5) < 70 && get_prox(0) < 70 && get_prox(7) < 70){		//left look
			move(s, FORWARD, 0);
			keep(5);
        	move(s, LEFT, 323);
        	stop();
        	if(color == get_color()){
        		move(s, FORWARD, 0);
        		keep(5);
            	move(s, RIGHT, 646);
            	stop();
            	chThdSleepMilliseconds(1500);
            	move(s, FORWARD, 0);
				keep(5);
	        	move(s, LEFT, 323);
        	} else {
            	chThdSleepMilliseconds(1500);
            	move(s, LEFT, 323);
        	}
			move(s, FORWARD, 0);
			keep(5);
        } else if(get_prox(2) < 70 && get_prox(0) < 70 && get_prox(7) < 70){		//right look
			move(s, FORWARD, 0);
			keep(5);
        	move(s, RIGHT, 323);
        	stop();
        	if(color == get_color()){
				move(s, FORWARD, 0);
				keep(5);
				move(s, LEFT, 646);
				stop();
				chThdSleepMilliseconds(1500);
				move(s, FORWARD, 0);
				keep(5);
				move(s, RIGHT, 323);
			} else {
				chThdSleepMilliseconds(1500);
				move(s, RIGHT, 323);
        }
    }
}
void move(int speed, int direction, int steps){		//move forwards or turn in the "direction" at "speed" for an amount of "steps"
	right_motor_set_pos(0);
	left_motor_set_pos(0);

	if(direction == FORWARD){
		right_motor_set_speed(speed);
		left_motor_set_speed(speed);
	}
	if(direction == RIGHT){
		right_motor_set_speed(-speed);
		left_motor_set_speed(speed);
	while(right_motor_get_pos() > -steps && left_motor_get_pos() < steps);
	}
	if(direction == LEFT){
		right_motor_set_speed(speed);
		left_motor_set_speed(-speed);
	while(right_motor_get_pos() < steps && left_motor_get_pos() > -steps);
	}
	set_body_led(0);
	set_front_led(1);
}

void keep(double distance){		//keep going forwards for "distance" in cm
	int steps = distance * 1000 / (2*M_PI*2.05);
	right_motor_set_pos(0);
	left_motor_set_pos(0);
	while(right_motor_get_pos() != steps && left_motor_get_pos() != steps);
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}

void calibration_motor(void){
	chThdSleepMilliseconds(3000);

	while(get_prox(0) > 80 || get_prox(7) > 80){
		move(300, RIGHT, 1);
	}

	while(get_prox(4) > get_prox(3)){
		move(50, RIGHT, 0);
	}

//	while(get_prox(3) > get_prox(4)){
//		move(50, LEFT, 0);
//	}
	stop();
}

void go_straight(void){
	if(get_prox(5) > get_prox(2) + 10){
		right_motor_set_speed(s-20);
		left_motor_set_speed(s+20);
	} else if(get_prox(5) + 10 < get_prox(2)){
		right_motor_set_speed(s+20);
		left_motor_set_speed(s-20);
	} else {
		right_motor_set_speed(s);
		left_motor_set_speed(s);
//		while(get_prox(1) > get_prox(6)){
//			move(50, LEFT, 0);
//		}
//		while(get_prox(6) > get_prox(1)){
//			move(50, RIGHT, 0);
//		}
	}
}
