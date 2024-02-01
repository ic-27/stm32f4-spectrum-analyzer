#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#define ARM_MATH_CM4
#define __FPU_PRESENT 1
#include "arm_math.h"

#include "executor.h"
#include "../bsp/led_matrix.h"

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

// set up SPI1
static void spi_setup(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
		    GPIO4 | GPIO5 | GPIO6 | GPIO7);
    gpio_set_af(GPIOA, GPIO_AF5, GPIO4 | GPIO5 | GPIO6 | GPIO7);

    rcc_periph_clock_enable(RCC_SPI1);
    spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_8,
			  SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
			  SPI_CR1_CPHA_CLK_TRANSITION_1,
			  SPI_CR1_DFF_8BIT,
			  SPI_CR1_LSBFIRST);
    
    spi_enable_ss_output(SPI1);
    spi_enable(SPI1);
}

int main(void)
{
    Executor exec = Executor();
    exec.init();
  
    int i;

    button_setup();
    gpio_setup();

    spi_setup();    
    spi_write(SPI1, 0x56);
    
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
