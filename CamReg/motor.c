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



void stop(void){
	right_motor_set_speed(0);
	left_motor_set_speed(0);
	set_body_led(1);
	set_front_led(0);
}

void move(int speed, int direction, int steps){
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

void keep(double distance){
	int steps = distance * 1000 / (2*M_PI*2.05);
	right_motor_set_pos(0);
	left_motor_set_pos(0);
	while(right_motor_get_pos() != steps && left_motor_get_pos() != steps);
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}

void calibration_motor(void){
	while(get_prox(0) >= 100){
		move(300, LEFT, 0);
	}
	while(get_prox(7) >= 100){
		move(300, RIGHT, 0);
	}
}
