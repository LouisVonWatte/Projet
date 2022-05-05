#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <msgbus/messagebus.h>
#include <i2c_bus.h>
#include <sensors/proximity.h>
#include <motors.h>
#include <leds.h>
#include <camera/po8030.h>
#include <chprintf.h>
#include <spi_comm.h>
#include <selector.h>
#include <chprintf.h>

#include <pi_regulator.h>
#include <process_image.h>

#define FORWARD 0
#define RIGHT 1
#define LEFT 2

#define WHEEL_RADIUS 2.05
#define WHEEL_PERIMETER 12.9	//about 1000 steps
#define ROBOT_SIZE 7.1

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

void SendUint8ToComputer(uint8_t* data, uint16_t size) 
{
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
}

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

void stop(void){
	right_motor_set_speed(0);
	left_motor_set_speed(0);
	set_body_led(1);
	set_front_led(0);
}

void move(int speed, int direction){
	right_motor_set_pos(0);
	left_motor_set_pos(0);

	if(direction == FORWARD){
		right_motor_set_speed(speed);
		left_motor_set_speed(speed);
	}
	if(direction == RIGHT){
		right_motor_set_speed(-speed);
		left_motor_set_speed(speed);
		while(right_motor_get_pos() > -325 && left_motor_get_pos() < 325);
	}
	if(direction == LEFT){
		right_motor_set_speed(speed);
		left_motor_set_speed(-speed);
		while(right_motor_get_pos() < 325 && left_motor_get_pos() > -325);
	}
	set_body_led(0);
	set_front_led(1);
}

void keep(double distance){
	int steps = distance * 1000 / WHEEL_PERIMETER;
	right_motor_set_pos(0);
	left_motor_set_pos(0);
	while(right_motor_get_pos() != steps && left_motor_get_pos() != steps);
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}

int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    //starts the serial communication
    serial_start();
    //start the USB communication
    usb_start();
    //starts the camera
    dcmi_start();
	po8030_start();
	//inits the motors
	motors_init();
	//start IR sensors
    proximity_start();
	//stars the threads for the pi regulator and the processing of the image
	pi_regulator_start();
	process_image_start();


	messagebus_init(&bus, &bus_lock, &bus_condvar);

	calibrate_ir();

    /* Infinite loop. */
    while (1) {

//    	static int led = 0;
////
//    	int s = get_selector();
//    	s++;
//    	s*=1100;
//    	s/=16;
//
//    	    	if(get_prox(2) >= 100 && get_prox(5) >= 100){
//    	    		stop();
//    	    		continue;
//    	    	}
//
//    	    	if(get_prox(2) >= 100){
//    	    		move(s, FORWARD);
//    	    	} else {
//    	    		move(s, FORWARD);
//    	    		keep(ROBOT_SIZE / 2.5);
//    	    		move(s, RIGHT);
//    	    		move(s, FORWARD);
//    	    		keep(ROBOT_SIZE);
//    	    	}
//
//    	    	if(get_prox(1) < 100){
//    	    		if(led <= 15000){
//    	    			set_led(LED3, 1);
//    	    			led++;
//    	    			continue;
//    	    		}
//    	    		if(led <= 30000 && led > 15000){
//    	    			set_led(LED3, 0);
//    	    			led++;
//    	    		}
//    	    		if(led > 30000){
//    	    			led = 0;
//    	    		}
//    	    	} else {
//    	    		set_led(LED3, 0);
//    	    		led = 0;
//    	    	}
//    	    	if(get_prox(5) >= 100 && get_prox(2) >= 100 && get_prox(0) >= 100){
//    				right_motor_set_speed(0);
//    				left_motor_set_speed(0);
//    				set_body_led(1);
//    				set_front_led(0);
//    				chThdSleepMilliseconds(2000);
//    				continue;
//    			}
//    			if(get_prox(5) >= 100){
//    				set_body_led(0);
//    				set_front_led(1);
//    				right_motor_set_speed(s);
//    				left_motor_set_speed(s);
//    				set_led(LED7, 1);
//    				chThdSleepMilliseconds(500);
//    				set_led(LED7, 0);
//    				chThdSleepMilliseconds(500);
//    				set_led(LED7, 1);
//    				chThdSleepMilliseconds(500);
//    				set_led(LED7, 0);
//    				chThdSleepMilliseconds(500);
//    				set_led(LED7, 1);
//    				right_motor_set_pos(0);
//    				left_motor_set_pos(0);
//    				right_motor_set_speed(s);
//    				left_motor_set_speed(-s);
//    				while(right_motor_get_pos() != 325 && left_motor_get_pos() != -325);
//    				clear_leds();
//    				continue;
//    			}
//    			if(get_prox(2) >= 100){
//    				set_body_led(0);
//    				set_front_led(1);
//    				right_motor_set_speed(s);
//    				left_motor_set_speed(s);
//    				set_led(LED3, 1);
//    				chThdSleepMilliseconds(500);
//    				set_led(LED3, 0);
//    				chThdSleepMilliseconds(500);
//    				set_led(LED3, 1);
//    				chThdSleepMilliseconds(500);
//    				set_led(LED3, 0);
//    				chThdSleepMilliseconds(500);
//    				set_led(LED3, 1);
//    				right_motor_set_pos(0);
//    				left_motor_set_pos(0);
//    				right_motor_set_speed(-s);
//    				left_motor_set_speed(s);
//    				while(right_motor_get_pos() != -325 && left_motor_get_pos() != 325);
//    				clear_leds();
//    				continue;
//    			}
//    			if(get_prox(0) >= 100 || get_prox(7) >= 100){
//    				set_body_led(0);
//    				set_front_led(1);
//    				right_motor_set_pos(0);
//    				left_motor_set_pos(0);
//    				right_motor_set_speed(s);
//    				left_motor_set_speed(s);
//    				while(right_motor_get_pos() != 325 && left_motor_get_pos() != 325);
//    				clear_leds();
//    				continue;
//    			}
//    			if(get_prox(3) >= 100){
//    				set_rgb_led(LED2, 0, 1, 0);
//    				set_rgb_led(LED4, 0, 1, 0);
//    				set_rgb_led(LED6, 0, 1, 0);
//    				set_rgb_led(LED8, 0, 1, 0);
//    				continue;
//    			}
//    			if(get_prox(4) >= 100){
//    				toggle_rgb_led(LED2, BLUE_LED, 1);
//    				toggle_rgb_led(LED4, BLUE_LED, 1);
//    				toggle_rgb_led(LED6, BLUE_LED, 1);
//    				toggle_rgb_led(LED8, BLUE_LED, 1);
//    				continue;
//    			}
//    			right_motor_set_speed(0);
//    			left_motor_set_speed(0);


//       int   rouge_max = 300 ; // le bit 6 pose probleme cad (64), lit vert et rouge a bit faible et que le rouge mais mal a bit fort
//       int   bleu_max = 300 ;   // ?????
//       int   vert_max = 100 ;  // vert et rouge se confondent




      int r = get_color_red();
      int b = get_color_blue();
      int v = get_color_green();
//       if( get_color_blue() > bleu_max) {
//           	    		set_led(LED7, 1);
//           	    		set_led(LED5, 0);
//           	    		set_led(LED3, 0);
//           	    		chThdSleepMilliseconds(2000);
//           	    		continue;
//           	    	}else if(get_color_red() > rouge_max) {
//           	    		set_led(LED3, 1);
//           	    		set_led(LED7, 0);
//           	    		set_led(LED5, 0);
//           	    	  //  chThdSleepMilliseconds(2000);
//           	    	    continue;
//           	    	} else if(get_color_green() > vert_max) {
//           	    	    set_led(LED5, 1);
//           	    	    set_led(LED7, 0);
//           	    	    set_led(LED3, 0);
//           	    	    chThdSleepMilliseconds(2000);
//           	    	    continue;
//           	    	}else{
//           	    	    clear_leds();
//           	    	}

       chprintf((BaseSequentialStream *)&SD3, "CAMERA\r\n");
       chprintf((BaseSequentialStream *)&SD3, "R=%3d, G=%3d, B=%3d\r\n\n", r, v, b);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
