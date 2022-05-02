#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <motors.h>
#include <camera/po8030.h>
#include <chprintf.h>
#include <sensors/proximity.h>
#include <msgbus/messagebus.h>


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
	// init IR sensors
	proximity_start();
	//stars the threads for the pi regulator and the processing of the image
	pi_regulator_start();
	process_image_start();

	messagebus_init(&bus, &bus_lock, &bus_condvar);

    /* Infinite loop. */
    while (1) {
    	//waits 1 second
  //  	proxy(&prox_values);
        chThdSleepMilliseconds(1000);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

/*void proxy(proximity_msg_t *prox_values){
	uint8_t led7 = 0;

	unsigned int *ambi = prox_values->ambient;
	unsigned int *refl = prox_values->reflected;

	if(ambi[PROXIMITY_NB_CHANNELS] !=0){
		led7 = 1;
	}
	 //to see the duration on the console
	 chprintf((BaseSequentialStream *)&SD3, "time = %dus\n",time);
	 //we invert the values because a led is turned on if the signal is low
	 palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);


*/

//}
void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
