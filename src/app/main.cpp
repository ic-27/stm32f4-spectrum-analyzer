#include <cstdint>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#define ARM_MATH_CM4
#define __FPU_PRESENT 1
#include "arm_math.h"

#include "ff.h"

#include "executor.h"
#include "../driver/oled.h"
#include "../bsp/led_matrix.h"

FIL file_h;

static void gpio_setup(void)
{
    /* Enable GPIOD clock. */
    rcc_periph_clock_enable(RCC_GPIOD);

    /* Set GPIO12 (in GPIO port D) to 'output push-pull'. */
    gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT,
		    GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14 | GPIO15);
}

static void button_setup(void)
{
    /* Enable GPIOA clock. */
    rcc_periph_clock_enable(RCC_GPIOA);
    
    /* Set GPIOA0 to 'input floating'. */
    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, GPIO0);
}

#define SD_CLK_PIN	GPIO13
#define SD_MISO_PIN	GPIO14
#define SD_MOSI_PIN	GPIO15
#define SD_SS_PIN	GPIO12

static void init_spi(void)
{
    rcc_periph_clock_enable(RCC_GPIOB);

    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
		    SD_CLK_PIN | SD_MISO_PIN | SD_MOSI_PIN);
    gpio_set_af(GPIOB, GPIO_AF5, SD_CLK_PIN | SD_MISO_PIN | SD_MOSI_PIN);

    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SD_SS_PIN); // ss
    gpio_set(GPIOB, SD_SS_PIN);

    rcc_periph_clock_enable(RCC_SPI2);
    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_32,
		    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
		    SPI_CR1_CPHA_CLK_TRANSITION_1,
		    SPI_CR1_DFF_8BIT,
		    SPI_CR1_MSBFIRST);
    
    spi_enable_ss_output(SPI2);
    spi_enable(SPI2);
}


int main(void)
{
    Executor exec = Executor();
    exec.init();
  
    int i;

    button_setup();
    gpio_setup();

    Oled oled = Oled();
    oled.init();

    #warning understand entirely how this works later
    FATFS fs;      
    const char path[] = "0";
    f_mount(&fs, path, 0);
    
    FRESULT fileStatus = f_open(&file_h, "help.txt", FA_OPEN_EXISTING | FA_READ);
    if (FR_OK != fileStatus) {
	while(1);
    }
    uint8_t buff[512];
    UINT bytes_read = 0;
    f_read(&file_h, buff, 10, &bytes_read);
    //init_spi();
    
    // gpio_clear(GPIOB, SD_SS_PIN);
    // spi_blocking_send(SPI2, 0x11);
    // gpio_set(GPIOB, SD_SS_PIN);
    
    
    /* Blink the LED (PD12) on the board. */
    while (1) {
	
	gpio_toggle(GPIOD, GPIO12);

	/* Upon button press, blink more slowly. */
	if (gpio_get(GPIOA, GPIO0)) {
	    for (i = 0; i < 3000000; i++) {	/* Wait a bit. */
		__asm__("nop");
	    }
	}

	for (i = 0; i < 3000000; i++) {		/* Wait a bit. */
	    __asm__("nop");
	}
    }

    return 0;
}
