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
#include <selector_color.h>

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

	messagebus_init(&bus, &bus_lock, &bus_condvar);

    serial_start(); 	//starts the serial communication
    usb_start();		//start the USB communication
    dcmi_start();		//starts the camera
	po8030_start();
	motors_init();  	//inits the motors
    proximity_start();  //start IR sensors
    playMelodyStart();  // start the melody
    dac_start();
    spi_comm_start();   // start the RGB led


	calibrate_ir();
	//stars the threads for the processing of the image and motor
	selector_color_start();
	process_image_start();

	int color = 0;
	int quit = 0;

	color = get_selector_color();

	calibration_motor();

	playMelody(MARIO_START, ML_SIMPLE_PLAY, NULL);
	waitMelodyHasFinished();

	set_body_led(0);
	set_front_led(1);

	while(quit == 0){
		go_straight();
		quit = check_turn(color);
	}

	if(quit == 1){
		playMelody(MARIO_FLAG, ML_SIMPLE_PLAY, NULL);
	}
	if(quit == 2){
		playMelody(MARIO_DEATH, ML_SIMPLE_PLAY, NULL);
	}
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
