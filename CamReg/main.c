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


int main(void)
{

    halInit();
    chSysInit();

    //starts the serial communication
    serial_start();
    //start the USB communication
    usb_start();
	//inits the motors
	motors_init();
	//start IR sensors
	proximity_start();
//	spi_comm_start();

	messagebus_init(&bus, &bus_lock, &bus_condvar);

	calibrate_ir();
//	calibrate_gyro();

    while (1) {

    			if(get_prox(5) >= 100 && get_prox(2) >= 100 && get_prox(0) >= 100){
    				right_motor_set_speed(0);
    				left_motor_set_speed(0);
    				set_body_led(1);
    				set_front_led(0);
    				chThdSleepMilliseconds(2000);
    				continue;
    			}
                if(get_prox(5) >= 100){
                	set_front_led(1);
					right_motor_set_speed(300);
					left_motor_set_speed(300);
                	set_led(LED7, 1);
    				chThdSleepMilliseconds(500);
                	set_led(LED7, 0);
    				chThdSleepMilliseconds(500);
                	set_led(LED7, 1);
    				chThdSleepMilliseconds(500);
                	set_led(LED7, 0);
    				chThdSleepMilliseconds(500);
    				set_led(LED7, 1);
					right_motor_set_pos(0);
					left_motor_set_pos(0);
					right_motor_set_speed(300);
					left_motor_set_speed(-300);
					while(right_motor_get_pos() != 325 && left_motor_get_pos() != -325);
	            	clear_leds();
					continue;
                }
                if(get_prox(2) >= 100){
                	set_front_led(1);
					right_motor_set_speed(300);
					left_motor_set_speed(300);
                	set_led(LED3, 1);
    				chThdSleepMilliseconds(500);
                	set_led(LED3, 0);
    				chThdSleepMilliseconds(500);
                	set_led(LED3, 1);
    				chThdSleepMilliseconds(500);
                	set_led(LED3, 0);
    				chThdSleepMilliseconds(500);
                	set_led(LED3, 1);
					right_motor_set_pos(0);
					left_motor_set_pos(0);
					right_motor_set_speed(-300);
					left_motor_set_speed(300);
					while(right_motor_get_pos() != -325 && left_motor_get_pos() != 325);
	            	clear_leds();
					continue;
                }
                if(get_prox(0) >= 100 || get_prox(7) >= 100){
        			set_body_led(0);
                	set_front_led(1);
					right_motor_set_pos(0);
					left_motor_set_pos(0);
					right_motor_set_speed(300);
					left_motor_set_speed(300);
					while(right_motor_get_pos() != 325 && left_motor_get_pos() != 325);
	            	clear_leds();
					continue;
                }
                if(get_prox(3) >= 100){
                	set_rgb_led(LED2, 0, 1, 0);
                	set_rgb_led(LED4, 0, 1, 0);
                	set_rgb_led(LED6, 0, 1, 0);
                	set_rgb_led(LED8, 0, 1, 0);
                	continue;
                }
                if(get_prox(4) >= 100){
                	toggle_rgb_led(LED2, BLUE_LED, 1);
                	toggle_rgb_led(LED4, BLUE_LED, 1);
                	toggle_rgb_led(LED6, BLUE_LED, 1);
                	toggle_rgb_led(LED8, BLUE_LED, 1);
                	continue;
                }
				right_motor_set_speed(0);
				left_motor_set_speed(0);

    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
