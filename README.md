* TODO SD card WAV FFT to 32x16 LED Matrix [30%]
- [X] Program stm32-disc1 through openocd
- [X] Set up Makefile for stm32-disc1
- [X] Integrate with cpputest
- [X] Set up debugging
- [ ] Write a mock
- [ ] SPI
- [ ] LED Matrix
- [ ] Integrate with ARM-CMSIS DSP library
- [ ] Buttons
- [ ] MicroSD card reader
- [ ] CMSIS FFT
- [ ] LED matrix
- [ ] Microphone
- [ ] Audio playback
* TODO: SD card WAV FFT to 2.2 TFT SPI 240x320 Display
* TODO: Microphone FFT to 2.2 TFT SPI 240x320 Display

- Libraries
https://github.com/abbrev/fatfs @commit b11f089

- Credit to links for deciphering how to set up various modules:
https://matheusmbar.com/bugfree-robot/2019/04/05/makefile-first-test.html
https://github.com/jwgrenning/cpputest-starter-project#run-tests-with-an-installed-tool-chain
https://github.com/ARM-software/CMSIS_4
https://www.youtube.com/watch?v=d1KvgOwWvkM
http://elm-chan.org/docs/mmc/mmc_e.html
http://elm-chan.org/docs/mmc/m/sdinit.png
https://embetronicx.com/tutorials/microcontrollers/stm32/stm32-sd-card-interfacing-with-example/#STM32_SD_Card_Example
https://os.mbed.com/users/mbed_official/code/SDFileSystem//file/8db0d3b02cec/SDFileSystem.cpp/
https://stackoverflow.com/questions/47041939/microsd-card-initialization-in-spi-mode-acmd41-always-returns-0x01/47071199#47071199
https://stackoverflow.com/questions/8080718/sdhc-microsd-card-and-spi-initialization
https://stackoverflow.com/questions/2365897/initializing-sd-card-in-spi-issues
https://www.youtube.com/watch?v=Mgh2WblO5_c
https://gaidi.ca/weblog/configuring-cmsis-dsp-package-and-performing-a-real-fft/