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

static int s = 400;

void stop(void){
	right_motor_set_speed(0);
	left_motor_set_speed(0);
	set_body_led(1);
	set_front_led(0);
}
static THD_WORKING_AREA(waMotorDirection, 2048);
static THD_FUNCTION(MotorDirection, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while  (1){
        if(get_prox(2) >= 100 && get_prox(5) >= 100){								//forwards if both walls
        	move(s, FORWARD, 0);
        } else if(get_prox(5) < 100 && get_prox(0) > 100 && get_prox(7) > 100){		//left if missing left wall
			move(s, FORWARD, 0);
			keep(4);
			move(s, LEFT, 323);
			move(s, FORWARD, 0);
			keep(7);
			set_led(LED7, 0);
        } else if(get_prox(2) < 100 && get_prox(0) > 100 && get_prox(7) > 100){		//right if missing right wall
			move(s, FORWARD, 0);
			keep(4);
			move(s, RIGHT, 323);
			move(s, FORWARD, 0);
			keep(7);
			set_led(LED3, 0);
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
void motor_start(void){
	chThdCreateStatic(waMotorDirection, sizeof(waMotorDirection), NORMALPRIO, MotorDirection, NULL);
}
