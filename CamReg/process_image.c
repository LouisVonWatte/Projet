#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>

#include <process_image.h>


static uint8_t color = 0;

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

// take an average of the RGB value on one line
uint8_t color_line(uint8_t *buffer){

	uint32_t mean_color = 0;

	for(uint16_t i =  (IMAGE_BUFFER_SIZE)/4 ; i < (IMAGE_BUFFER_SIZE)*3/4  ; i++){
		mean_color += buffer[i];
	}

	mean_color /= IMAGE_BUFFER_SIZE;

	return mean_color;
}


static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	//Takes pixels 0 to IMAGE_BUFFER_SIZE  (minimum 2 lines because reasons)
	po8030_advanced_config(FORMAT_RGB565, 0, 10, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
	po8030_set_awb(0); //calibration of exposition
	dcmi_enable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

    while(1){
        //starts a capture
		dcmi_capture_start();
		//waits for the capture to be done
		wait_image_ready();
		//signals an image has been captured
		chBSemSignal(&image_ready_sem);

    }
}

static THD_WORKING_AREA(waProcessImage, 2048);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	uint8_t *img_buff;
	uint8_t image_red[IMAGE_BUFFER_SIZE], image_blue[IMAGE_BUFFER_SIZE], image_green[IMAGE_BUFFER_SIZE] = {0};
    while(1){
    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);
        img_buff  = dcmi_get_last_image_ptr();

		for(uint16_t i = 0 ; i < 2*(IMAGE_BUFFER_SIZE) ; i+=2){
			//extracts red blue and green form the buffers
			image_red[i/2] = (uint8_t)img_buff[i]&0xF8;
			image_blue[i/2] = (((uint8_t)img_buff[i+1]&0x1F) <<3 );
			image_green[i/2] = (((uint8_t)img_buff[i]&0x07)<< 5) | (((uint8_t)img_buff[i+1]&0xE0)>> 3);
		}

		if( (color_line(image_red)-color_line(image_blue))&&(color_line(image_red)-color_line(image_green)) > 30 ) {
			color = 1; //red
		continue;
		}else if( (color_line(image_green)-color_line(image_red))&&(color_line(image_green)-color_line(image_blue)) > 5 ) {
			color = 2; // green
		continue;
		} else if( (color_line(image_blue)-color_line(image_red))&&(color_line(image_blue)-color_line(image_green)) > 30 ) {
			color = 3; // blue
		continue;
		}else{
			color = 0;
		}
    }
}
uint8_t get_color(void){
	return color;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}
