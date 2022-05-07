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

#include <pi_regulator.h>
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
	//stars the threads for the pi regulator and the processing of the image
//	pi_regulator_start();
	process_image_start();


	messagebus_init(&bus, &bus_lock, &bus_condvar);

	calibrate_ir();
//	calibration_motor();

	dac_start();
    playMelodyStart();
	spi_comm_start();

    while (1) {
//
//    	static int led = 0;
//
//    	int s = get_selector();
//    	s++;
//    	s*=1100;
//    	s/=16;
//
//    	if(get_prox(2) >= 100 && get_prox(5) >= 100){
//    		stop();
//    		continue;
//    	}
//
//    	if(get_prox(2) >= 100){
//    		move(s, FORWARD, 0);
//    	} else {
//    		move(s, FORWARD, 0);
//    		keep(7 / 2);
//    		move(s, RIGHT, 323);
//    		move(s, FORWARD, 0);
//    		keep(7);
//    		set_led(LED3, 0);
//    	}
//
//    	if(get_prox(1) < 100){
//    		if(led <= 15000){
//    			set_led(LED3, 1);
//    			led++;
//    			continue;
//    		}
//    		if(led <= 30000 && led > 15000){
//    			set_led(LED3, 0);
//    			led++;
//    		}
//    		if(led > 30000){
//    			led = 0;
//    		}
//    	} else {
//    		set_led(LED3, 0);
//    		led = 0;
//    	}

//		set_rgb_led(0, 30, 20, 10);
//		set_rgb_led(1, 10, 20, 30);
//		set_rgb_led(2, 0, 30, 10);
//		set_rgb_led(3, 30, 0, 30);

    	int r = get_color_red();
    	int v = get_color_green();
    	int b = get_color_blue();




       //int   diff = 20 ; // le bit 6 pose probleme cad (64), lit vert et rouge a bit faible et que le rouge mais mal a bit fort
//       int   bleu_max = 30 ;   // ?????
//       int   vert_max = 30 ;  // vert et rouge se confondent

       if( (b-r)&&(b-v) > 30 ) {
           	    		set_rgb_led(0, 0, 0, 15);
           	    		set_rgb_led(1, 0, 0, 0);
           	    		set_rgb_led(2, 0, 0, 0);
           	    	//	chThdSleepMilliseconds(2000);
           	    		continue;
           	    	}else if( (r-b)&&(r-v) > 20 ) {
           	    		set_rgb_led(0, 0, 0, 0);
           	    		set_rgb_led(1, 15, 0, 0);
           	    		set_rgb_led(2, 0, 0, 0);
		           //	    chThdSleepMilliseconds(2000);
		           	    continue;
           	    	} else if( (v-r)&&(v-b) > 10 ) {
           	    		set_rgb_led(0, 0, 0, 0);
           	    		set_rgb_led(1, 0, 0, 0);
           	    		set_rgb_led(2, 0, 15, 0);
		           //   chThdSleepMilliseconds(2000);
		           	   continue;
           	    	}else{
           	    	    clear_leds();
           	    	}

//                           if( (b-v)&&(r-v) > diff ) {
//                 	    		set_rgb_led(0, 15, 0, 15);
//                 	   //	chThdSleepMilliseconds(2000);
//                 	    		continue;
//                 	    	}else if( (r-b)&&(v-b) > diff ) {
//                 	    		set_rgb_led(1, 15, 15, 0);
//      		           //	    chThdSleepMilliseconds(2000);
//      		           	    continue;
//                 	    	} else if( (v-r)&&(b-r) > 20 ) {
//                 	    	   set_rgb_led(2, 0, 15, 15);
//      		           //   chThdSleepMilliseconds(2000);
//      		           	   continue;
//                 	    	}else{
//                 	    	    clear_leds();
//                 	    	}




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
