#include "spectrum_analyzer.h"
#include "display.h"

Spec::Spec(void)
{
    this->init();
}

/**
 * init() - Initialize oled display, filesystem, arm fft
 *
 * Return: void
 */
void Spec::init(void)
{
    display = Display();
    
    #warning check the return values
    f_mount(&Fatfs, "", 0);
    FRESULT fileStatus = f_open(&file_h, WAV_FILE_NAME, FA_OPEN_EXISTING | FA_READ);
    f_lseek(&file_h, WAV_METADATA_LEN); // ignore any metadata
    arm_status armStat = arm_rfft_fast_init_f32(&s, FFTLEN); 
}

/**
 * calcFFT()
 *
 * Calculate FFT of audio samples, calc magnitude, and display to oled screen
 * Return: void
 */
void Spec::calcFFT(void)
{
    FRESULT fileStatus;
    UINT bytesRead = 0;
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

#warning comment what frequencies the frequency bands represent
	    // put into different frequency bands
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
	
	// map each frequency band between 0 and number of rows on the oled screen
	uint8_t freqBandScaled[8] = {0};
	for (int i=0; i<8; ++i) {
	    freqBandScaled[i] = (uint8_t)(freqBand[i]*SCALE_FACTOR);
	    if (freqBandScaled[i] > OLED_ROWS) {
		freqBandScaled[i] = OLED_ROWS;
	    }
	}
#warning use a proper timer!
	for(int i=0; i<500000; ++i); // wait for some time
	// output to oled screen
	display.update(freqBandScaled);

	// reset
	memset(freqBand, 0, sizeof(freqBand));
    } while ((FR_OK == fileStatus) && (bytesRead == FFTLEN*2));
}
