#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>

#include <process_image.h>


static int red, blue, green  = 0;
static uint16_t line_position = IMAGE_BUFFER_SIZE/2;	//middle

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

/*
 *  Returns the line's width extracted from the image buffer given
 *  Returns 0 if line not found
 */

uint8_t color_line(uint8_t *buffer){

	uint32_t mean_color = 0;

	for(uint16_t i = IMAGE_BUFFER_SIZE/4  ; i < ( IMAGE_BUFFER_SIZE*3/4)  ; i++){
			mean_color += buffer[i];
		}
		mean_color /= IMAGE_BUFFER_SIZE;

	return mean_color;
}


static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	//Takes pixels 0 to IMAGE_BUFFER_SIZE of the line 10 + 11 (minimum 2 lines because reasons)
	po8030_advanced_config(FORMAT_RGB565, 0, 10, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
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

	uint8_t *img_buff_1;
	uint8_t *img_buff_2;
	uint8_t image_red[IMAGE_BUFFER_SIZE], image_blue[IMAGE_BUFFER_SIZE], image_green[IMAGE_BUFFER_SIZE] = {0};
    while(1){
    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);
        img_buff_1  = dcmi_get_last_image_ptr();
//        img_buff_1 = dcmi_get_first_buffer_ptr();
        img_buff_2 = dcmi_get_second_buffer_ptr();


		for(uint16_t i = 0 ; i < ( 2*IMAGE_BUFFER_SIZE) ; i+=2){
			//extracts first 5bits of the first byte
			//takes nothing from the second byte
			image_red[i/2] = (uint8_t)img_buff_1[i]&0xF8;
			image_blue[i/2] = (((uint8_t)img_buff_2[i]&0x1F)<< 5);
			image_green[i/2] = (((uint8_t)img_buff_1[i]&0x07)<< 5) | (((uint8_t)img_buff_2[i]&0xE0)>> 3);


		}

      red = color_line(image_red);
      blue = color_line(image_blue); //color
      green = color_line(image_green);
    }
}

int get_color_red(void){
	return red;
}
int get_color_blue(void){
	return blue;
}
int get_color_green(void){
	return green;
}

uint16_t get_line_position(void){
	return line_position;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}
