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

#define MAX_INT16 32767

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

    Oled oled = Oled();
    oled.init();

    FATFS Fatfs; // work area for logical drive
    f_mount(&Fatfs, "", 0);

#warning check the status
    FRESULT fileStatus = f_open(&file_h, "10kHz.wav", FA_OPEN_EXISTING | FA_READ);

    // read the wav file in buffer
    int16_t readBuff[FFTLEN]; // 16-bit audio
    UINT bytesRead = 0;
    f_lseek(&file_h, WAV_METADATA_LEN); // ignore any metadata
    
    float32_t fftIn[FFTLEN]={0}, fftOut[FFTLEN]={0}, realFFT[FFTLEN/2]={0}, imagFFT[FFTLEN/2]={0};
    float32_t powerFFT[FFTLEN/2]={0};
    float32_t freqBand[8] = {0};

    arm_status armStat = arm_rfft_fast_init_f32(&s, FFTLEN); 
    
    do {
	fileStatus = f_read(&file_h, readBuff, FFTLEN*2, &bytesRead); // read wav file

	// convert read wav file data into fft input, must be float32_t datatype
	for (uint16_t j=0; j<FFTLEN; ++j) { 
	    fftIn[j] = (float32_t)(readBuff[j]);
	}
	arm_rfft_fast_f32(&s, fftIn, fftOut, 0);

	//float32_t peakVal = 0; // calculate the peakHz
	//uint32_t peakHz = 0;

	// calculate the magnitude in store into frequency bands
	uint32_t freqBinIndex = 0;
	for (uint32_t j=0; j<(FFTLEN/2)-1; ++j) {
	    realFFT[j] = fftOut[j*2];
	    imagFFT[j] = fftOut[(j*2)+1];

	    // calculate the magnitude
	    float32_t mag = sqrtf((realFFT[j]*realFFT[j]) + (imagFFT[j]*imagFFT[j]));
	    
	    // scale magnitude between 0 and 1
	    float32_t scaled_mag = (mag*2/(MAX_INT16*FFTLEN));
	    powerFFT[j] = scaled_mag;

	    // put into frequency bands
	    if (freqBinIndex <= 1) {
		freqBand[0] += scaled_mag;
	    } else if (freqBinIndex > 1 && freqBinIndex <= 3) {
		freqBand[1] += scaled_mag;
	    } else if (freqBinIndex > 3 && freqBinIndex <= 7) {
		freqBand[2] += scaled_mag;
	    } else if (freqBinIndex > 3 && freqBinIndex <= 14) {
		freqBand[3] += scaled_mag;
	    } else if (freqBinIndex > 14 && freqBinIndex <= 30) {
		freqBand[4] += scaled_mag;
	    } else if (freqBinIndex > 30 && freqBinIndex <= 64) {
		freqBand[5] += scaled_mag;
	    } else if (freqBinIndex > 64 && freqBinIndex <= 136) {
		freqBand[6] += scaled_mag;
	    } else if (freqBinIndex > 136 && freqBinIndex <= 291) {
		freqBand[7] += scaled_mag;
	    }
	    ++freqBinIndex;
	}
	// map between 0 and OLED_SCREEN_ROWS (64)
	
	// output to oled screen
	
	memset(freqBand, 0, sizeof(freqBand));
    } while ((FR_OK == fileStatus) && (bytesRead == FFTLEN*2));


#warning how do I place values into certain bins
    // each index represents a certain frequency, so we need to do it based on the index
    // get the average of indexes in a certain bin, 
    
    
    
    // arm_cmplx_mag_squared_f32(fftOut, powerFFT, FFTLEN/2);
    // uint32_t maxIndex=0;
    // float32_t maxValue=0;
    // arm_max_f32(&powerFFT[1], (FFTLEN/2)-1, &maxValue, &maxIndex);


    while (1) {
	
    }

    return 0;
}
