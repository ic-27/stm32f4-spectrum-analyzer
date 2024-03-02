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
    
    float32_t fftIn[FFTLEN]={0}, fftOut[FFTLEN]={0};
    float32_t realFFT[FFTLEN/2]={0}, imagFFT[FFTLEN/2]={0};
    float32_t freqBand[8] = {0};

    arm_status armStat = arm_rfft_fast_init_f32(&s, FFTLEN); 
    
    do {
	fileStatus = f_read(&file_h, readBuff, FFTLEN*2, &bytesRead); // read wav file

	// convert read wav file data into fft input, must be float32_t datatype
	for (uint16_t j=0; j<FFTLEN; ++j) { 
	    fftIn[j] = (float32_t)(readBuff[j]);
	}
	arm_rfft_fast_f32(&s, fftIn, fftOut, 0);

	// calculate the magnitude in store into frequency bands
	uint32_t freqBinIndex = 0;
	for (uint32_t j=0; j<(FFTLEN/2)-1; ++j) {
	    realFFT[j] = fftOut[j*2];
	    imagFFT[j] = fftOut[(j*2)+1];

	    // calculate the magnitude
	    float32_t mag = sqrtf((realFFT[j]*realFFT[j]) + (imagFFT[j]*imagFFT[j]));
	    
	    // scale magnitude between 0 and 1
	    float32_t scaled_mag = (mag*2/(MAX_INT16*FFTLEN));

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
	
	// map each frequency band between 0 and OLED_SCREEN_ROWS (64)
	uint8_t freqBandScaled[8] = {0};
	for (int i=0; i<8; ++i) {
	    freqBandScaled[i] = (uint8_t)(freqBand[i]*64); // 18 is the scale factor, found to work across different wav files
	    if (freqBandScaled[i] > OLED_ROWS) {
		freqBandScaled[i] = OLED_ROWS;
	    }
	}
	for(int i=0; i<100000; ++i); // wait >= 1ms
	// output to oled screen
	oled.update(freqBandScaled);

	// reset
	memset(freqBand, 0, sizeof(freqBand));
    } while ((FR_OK == fileStatus) && (bytesRead == FFTLEN*2));

    while (1) {
	
    }

    return 0;
}
