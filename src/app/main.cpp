#include <cstdint>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include "math.h"

#define ARM_MATH_CM4
#define __FPU_PRESENT 1
#include "arm_math.h"

#include "ff.h"

#include "executor.h"
#include "../driver/oled.h"
#include "../bsp/led_matrix.h"

#define WAV_METADATA_LEN 44
#define FFTLEN 512

FIL file_h;
arm_rfft_fast_instance_f32 s;

// static void gpio_setup(void)
// {
//     /* Enable GPIOD clock. */
//     rcc_periph_clock_enable(RCC_GPIOD);

//     /* Set GPIO12 (in GPIO port D) to 'output push-pull'. */
//     gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT,
// 		    GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14 | GPIO15);
// }

// static void button_setup(void)
// {
//     /* Enable GPIOA clock. */
//     rcc_periph_clock_enable(RCC_GPIOA);
    
//     /* Set GPIOA0 to 'input floating'. */
//     gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, GPIO0);
// }

int main(void)
{
    Executor exec = Executor();
    exec.init();
  
    //int i;
    // button_setup();
    // gpio_setup();

    Oled oled = Oled();
    oled.init();

    FATFS Fatfs; // work area for logical drive
    f_mount(&Fatfs, "", 0);

#warning check the status
    FRESULT fileStatus = f_open(&file_h, "1kHz.wav", FA_OPEN_EXISTING | FA_READ);

    // read the wav file in buffer
    int16_t readBuff[FFTLEN]; // 16-bit audio
    UINT bytesRead = 0;
    f_lseek(&file_h, WAV_METADATA_LEN); // ignore any metadata
    
    float32_t fftIn[FFTLEN]={0}, fftOut[FFTLEN]={0}, powerFFT[FFTLEN/2]={0};
    float32_t realFFT[FFTLEN/2]={0}, imagFFT[FFTLEN/2]={0};

    arm_status armStat = arm_rfft_fast_init_f32(&s, FFTLEN); 
    
    do {
	fileStatus = f_read(&file_h, readBuff, FFTLEN*2, &bytesRead); // read wav file

	for (uint16_t j=0; j<FFTLEN; ++j) { // convert read wav file data into fft input
	    fftIn[j] = (float32_t)(readBuff[j]);
	}
	
	arm_rfft_fast_f32(&s, fftIn, fftOut, 0); // perform FFT

	float32_t peakVal = 0; // calculate the peakHz
	uint32_t peakHz = 0;
	uint32_t freqIndex = 0;
	for (uint32_t j=0; j<(FFTLEN/2)-1; ++j) {
	    realFFT[j] = fftOut[j*2];
	    imagFFT[j] = fftOut[(j*2)+1];

#warning how to constrain the magnitude to be between 0 and 1 (0 and 64)
	    // calculate the magnitude
	    float32_t curMag = sqrtf((realFFT[j]*realFFT[j]) + (imagFFT[j]*imagFFT[j]));
	    if (curMag > peakVal) {
		peakVal = curMag;
		peakHz = (uint16_t)(freqIndex*44100/((float32_t)FFTLEN));
	    }
	    ++freqIndex;
	}
	freqIndex = 0; // filler!!!
    } while ((FR_OK == fileStatus) && (bytesRead == FFTLEN*2));


#warning how do I place values into certain bins
    // each index represents a certain frequency, so we need to do it based on the index
    // get the average of indexes in a certain bin, 
    
    
    
    // arm_cmplx_mag_squared_f32(fftOut, powerFFT, FFTLEN/2);
    // uint32_t maxIndex=0;
    // float32_t maxValue=0;
    // arm_max_f32(&powerFFT[1], (FFTLEN/2)-1, &maxValue, &maxIndex);

    
    /* Blink the LED (PD12) on the board. */
    while (1) {
	
	// gpio_toggle(GPIOD, GPIO12);

	// /* Upon button press, blink more slowly. */
	// if (gpio_get(GPIOA, GPIO0)) {
	//     for (i = 0; i < 3000000; i++) {	/* Wait a bit. */
	// 	__asm__("nop");
	//     }
	// }

	// for (i = 0; i < 3000000; i++) {		/* Wait a bit. */
	//     __asm__("nop");
	// }
    }

    return 0;
}
