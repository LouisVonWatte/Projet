#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <msgbus/messagebus.h>
#include <i2c_bus.h>
#include <sensors/proximity.h>
#include <motors.h>
#include <motor.h>
#include <leds.h>
#include <camera/po8030.h>
#include <spi_comm.h>
#include <selector.h>
#include <audio/play_melody.h>
#include <audio/audio_thread.h>
#include <process_image.h>

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
	//stars the threads for the processing of the image and ...(motor)
	process_image_start();

	messagebus_init(&bus, &bus_lock, &bus_condvar);

	calibrate_ir();
//	calibration_motor();

	dac_start();
    playMelodyStart();
	spi_comm_start();

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
			break;
		}
		if(s_vert >= 1000000){
			set_rgb_led(0, 0, 15, 0);
			set_rgb_led(1, 0, 15, 0);
			set_rgb_led(2, 0, 15, 0);
			set_rgb_led(3, 0, 15, 0);
			break;
		}
		if(s_bleu >= 1000000){
			set_rgb_led(0, 0, 0, 15);
			set_rgb_led(1, 0, 0, 15);
			set_rgb_led(2, 0, 0, 15);
			set_rgb_led(3, 0, 0, 15);
			break;
		}
	}

    while (1) {

//    	static int led_l = 0;
//    	static int led_r = 0;


//    	if(get_prox(2) >= 100 && get_prox(5) >= 100){
//    		move(s, FORWARD, 0);
//    	} else if(get_prox(5) < 100){
//    		move(s, FORWARD, 0);
//    		keep(4);
//    		move(s, LEFT, 323);
//    		move(s, FORWARD, 0);
//    		keep(7);
//    		set_led(LED7, 0);
//    	} else if(get_prox(2) < 100){
//    		move(s, FORWARD, 0);
//    		keep(4);
//    		move(s, RIGHT, 323);
//    		move(s, FORWARD, 0);
//    		keep(7);
//    		set_led(LED3, 0);
//    	}

//    	if(get_prox(6) < 100){
//    		if(led_l <= 15000){
//    			set_led(LED7, 1);
//    			led_l++;
//    			continue;
//    		}
//    		if(led_l <= 30000 && led_l > 15000){
//    			set_led(LED7, 0);
//    			led_l++;
//    		}
//    		if(led_l > 30000){
//    			led_l = 0;
//    		}
//    	} else {
//    		set_led(LED7, 0);
//    		led_l = 0;
//    	}
//
//    	if(get_prox(1) < 100){
//    		if(led_r <= 15000){
//    			set_led(LED3, 1);
//    			led_r++;
//    			continue;
//    		}
//    		if(led_r <= 30000 && led_r > 15000){
//    			set_led(LED3, 0);
//    			led_r++;
//    		}
//    		if(led_r > 30000){
//    			led_r = 0;
//    		}
//    	} else {
//    		set_led(LED3, 0);
//    		led_r = 0;
//    	}

    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
