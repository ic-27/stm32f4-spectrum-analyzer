#ifndef SPEC_H
#define SPEC_H

#define ARM_MATH_CM4
#define __FPU_PRESENT 1
#include "arm_math.h"

#include "ff.h"
#include "display.h"

#define WAV_FILE_NAME "10kHz.wav"

#define WAV_METADATA_LEN 44 // skip these number of bytes
#define FFTLEN 512
#define MAX_INT16 32767

#define SCALE_FACTOR 64 // scale factor for magnitude to oled screen; may change

class Spec {
 private:
    Display display;
    
    FATFS Fatfs; // work area for logical drive
    FIL file_h;

    int16_t readBuff[FFTLEN]; // 16-bit audio
    float32_t fftIn[FFTLEN], fftOut[FFTLEN];
    float32_t realFFT[FFTLEN/2], imagFFT[FFTLEN/2];
    float32_t freqBand[8];

    arm_rfft_fast_instance_f32 s;
    
 public:
    Spec(void);
    void init(void);
    void calcFFT(void);
};

#endif
