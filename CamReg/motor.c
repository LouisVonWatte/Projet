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

int check_turn (int color){

	if(get_prox(5) < 100 && get_prox(0) > 100 && get_prox(7) > 100){		//left if missing left wall
		while(get_prox(7) < 400){
			move(s, FORWARD, 0);
		}
		move(s, LEFT, 323);
		move(s, FORWARD, 0);
		keep(6);
	} else if(get_prox(2) < 100 && get_prox(0) > 100 && get_prox(7) > 100){		//right if missing right wall
		while(get_prox(0) < 400){
			move(s, FORWARD, 0);
		}
		move(s, RIGHT, 323);
		move(s, FORWARD, 0);
		keep(6);
	} else if(get_prox(5) < 150 && get_prox(0) < 80 && get_prox(7) < 80){		//left look
		move(s, FORWARD, 0);
		keep(4);
		move(s, LEFT, 323);
		stop();
		chThdSleepMilliseconds(1000);
		if(color == get_color()){
			move(s, FORWARD, 0);
			keep(9);
			move(s, RIGHT, 646);
			stop();						//make a full stop
			return 1;
		} else {
			chThdSleepMilliseconds(1500);
			move(s, RIGHT, 323);
			move(s, FORWARD, 0);
			keep(5);
		}
	} else if(get_prox(2) < 150 && get_prox(0) < 80 && get_prox(7) < 80){		//right look
		move(s, FORWARD, 0);
		keep(4);
		move(s, RIGHT, 323);
		stop();
		chThdSleepMilliseconds(1000);
		if(color == get_color()){
			move(s, FORWARD, 0);
			keep(9);
			move(s, LEFT, 646);
			stop();		//make a full stop
			return 1;
		} else {
			chThdSleepMilliseconds(1500);
			move(s, LEFT, 323);
			move(s, FORWARD, 0);
			keep(5);
		}
	} else if(get_prox(0) > 200 && get_prox(1) > 200 && get_prox(2) > 200 && get_prox(6) > 200 && get_prox(6) > 200 && get_prox(7) > 200){
		set_body_led(1);
		set_front_led(0);
		set_rgb_led(0, 15, 15, 15);
		set_rgb_led(1, 15, 15, 15);
		set_rgb_led(2, 15, 15, 15);
		set_rgb_led(3, 15, 15, 15);
		stop();
		return 2;
	}
    return 0;
}
void move(int speed, int direction, int steps){		//move forwards or turn in the "direction" at "speed" for an amount of "steps"
	right_motor_set_pos(0);
	left_motor_set_pos(0);
	set_body_led(0);
	set_front_led(1);
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
	chThdSleepMilliseconds(2000);

	while(get_prox(0) > 100 || get_prox(7) > 100){
		move(300, RIGHT, 1);
	}

	while(get_prox(6) > get_prox(1)){
		move(50, RIGHT, 0);
	}
//	while(get_prox(3) > get_prox(4)){
//		move(50, LEFT, 0);
//	}
	stop();
}

void go_straight(void){
	if(get_prox(5) + get_prox(6) > get_prox(2) + get_prox(1) + 100){
		right_motor_set_speed(s-20);
		left_motor_set_speed(s+20);
	} else if(get_prox(5) + get_prox(6) + 100 < get_prox(2) + get_prox(1)){
		right_motor_set_speed(s+20);
		left_motor_set_speed(s-20);
	} else {
		right_motor_set_speed(s);
		left_motor_set_speed(s);
	}
}
