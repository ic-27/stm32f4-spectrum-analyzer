## Overview ##
This is an audio spectrum analyzer written in C/C++ using FFT of wav audio file read from microSD card through FatFS displayed onto an OLED screen. Credits to [anweshct3](https://github.com/anweshct3/Audio-Spectrum-Analyzer-STM32) for initial project inspiration. This project also stemmed from my wanting to learn/practice C++, DSP, ARM, CppUTest, and integrating different libraries.

## Task List ## 
  - [x] Write audio spectrum analyzer in python
  - [x] Integrate libopencm3 open source HAL
  - [x] Set up programming stm32f4-disc1 through openocd
  - [x] Set up Makefile for stm32f4-disc1
  - [x] Integrate with ARM-CMSIS library (DSP
  - [x] Set up debugging for stm32f4-disc1
  - [x] Integrate CppUTest for unit/integration testing
  - [x] Learn how to use CppUTest to write a mock
  - [x] Write middleware for FatFS to integrate w/ microSD card
  - [x] Write module for OLED driver
  - [x] Test CMSIS-4 DSP functions with audio wav file
  - [ ] Display DSP output onto OLED
  - [ ] Use I2C to allow audio playback

## Demo ##
Coming soon! :-)

## Structure ##
```
build: makefile for building and flashing project
test:  makefile for running unit & integration tests using CppUTest
lib:   external libraries
src:   contains src and header files for building the project split into application, bsp, and driver
misc:  audio spectrum analyzer written in python3 & wav files
```

## How to Build ##
Coming soon! :-)

## Libraries ##
* [libopencm3](https://libopencm3.org/)
* [CMSIS-4](https://github.com/ARM-software/CMSIS_4)
* [FatFS](https://github.com/abbrev/fatfs)
* [CppUTest](https://github.com/cpputest/cpputest)

## Helpful Links ##
* [[CppUTest] How to write a makefile for CppUTest](https://matheusmbar.com/bugfree-robot/2019/04/05/makefile-first-test.html)
* [[CppUTest] CppUTest Starter Project](https://github.com/jwgrenning/cpputest-starter-project#run-tests-with-an-installed-tool-chain)
* [[FFT] CMSIS-4 Library](https://github.com/ARM-software/CMSIS_4)
* [[FFT] STM32 Fast Fourier Transform (CMSIS DSP FFT) - Phil's Lab #111](https://www.youtube.com/watch?v=d1KvgOwWvkM)
* [[FFT] ESP32 spectrum analyser VU meter using arduinoFFT and a FastLED matrix](https://www.youtube.com/watch?v=Mgh2WblO5_c)
* [[FFT] Configuring CMSIS DSP Package and Performing a Real FFT](https://gaidi.ca/weblog/configuring-cmsis-dsp-package-and-performing-a-real-fft/)
* [[FatFS] How to integrate FatFS using SPI](http://elm-chan.org/docs/mmc/mmc_e.html)
* [[FatFS] FatFS SD card Initialization Process](http://elm-chan.org/docs/mmc/mmc_e.html)
* [[FatFS] Example integration FatFS on stm32](https://embetronicx.com/tutorials/microcontrollers/stm32/stm32-sd-card-interfacing-with-example/#STM32_SD_Card_Example)
* [[FatFS] Example #2 integration FatFS on stm32](https://os.mbed.com/users/mbed_official/code/SDFileSystem//file/8db0d3b02cec/SDFileSystem.cpp/)
* [[FatFS] microSD card initialization in SPI mode. ACMD41 always returns 0x01](https://stackoverflow.com/questions/47041939/microsd-card-initialization-in-spi-mode-acmd41-always-returns-0x01/47071199#47071199)
* [[FatFS] SDHC microSD card and SPI initialization](https://stackoverflow.com/questions/8080718/sdhc-microsd-card-and-spi-initialization)
* [[FatFS] Initializing SD card in SPI issues](https://stackoverflow.com/questions/2365897/initializing-sd-card-in-spi-issues)
