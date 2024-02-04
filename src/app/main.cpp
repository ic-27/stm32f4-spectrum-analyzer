#include <cstdint>
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
		    GPIO5 | GPIO6 | GPIO7);
    gpio_set_af(GPIOA, GPIO_AF5, GPIO5 | GPIO6 | GPIO7);

    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4); // ss
    gpio_set(GPIOA, GPIO4);

    rcc_periph_clock_enable(RCC_SPI1);
    spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_32,
		    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
		    SPI_CR1_CPHA_CLK_TRANSITION_1,
		    SPI_CR1_DFF_8BIT,
		    SPI_CR1_MSBFIRST);
    
    spi_enable_ss_output(SPI1);
    spi_enable(SPI1);
}

#define OLED_ROWS 128
#define OLED_COLS 64
#define OLED_PAGE_START 0xB0
#define OLED_PAGE_END 0xB7

void OLED_WR_CMD(uint8_t data)
{
    // DC pin set to 0 for CMD
    gpio_clear(GPIOA, GPIO0);
    
    // send through SPI
    gpio_clear(GPIOA, GPIO4);
    spi_blocking_send(SPI1, data);
    gpio_set(GPIOA, GPIO4);
}

void OLED_WR_DATA(uint8_t data)
{
    // DC pin set to 1 for DATA
    gpio_set(GPIOA, GPIO0);
    
    // send through SPI
    gpio_clear(GPIOA, GPIO4);
    spi_blocking_send(SPI1, data);
    gpio_set(GPIOA, GPIO4);
}

/**
 * OLED_Clear() - Clear OLED screen
 * 
 * Return: void
 */
void OLED_Clear(void)
{
    for (uint8_t j=OLED_PAGE_START; j<=OLED_PAGE_END; ++j) {
	OLED_WR_CMD(j); // set page
	OLED_WR_CMD(0x02); // set lower column addr
	OLED_WR_CMD(0x10); // set higher column addr
	for (uint8_t k=0; k<OLED_ROWS; ++k) {
	    OLED_WR_DATA(0x00);
	}
    }
}


void OLED_Init(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0); // dc
    gpio_clear(GPIOA, GPIO0); // cmd
    
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2); // rst
    
    //OLED 复位
    // OLED_RES_Clr();//RES 置 0
    gpio_clear(GPIOA, GPIO2);
    //delay_ms(200);//延时 200ms
    for (uint32_t i=0; i<3000000; ++i) {__asm__("nop");}
    // OLED_RES_Set();//RES 置 1
    gpio_set(GPIOA, GPIO2);
    
    //OLED 初始化
    OLED_WR_CMD(0xAE); /*display off*/
    OLED_WR_CMD(0x02); /*set lower column address*/
    OLED_WR_CMD(0x10); /*set higher column address*/
    OLED_WR_CMD(0x40); /*set display start line*/
    OLED_WR_CMD(0xB0); /*set page address*/
    OLED_WR_CMD(0x81); /*contract control*/
    OLED_WR_CMD(0xcf); /*128*/
    OLED_WR_CMD(0xA1); /*set segment remap*/
    OLED_WR_CMD(0xA6); /*normal / reverse*/
    OLED_WR_CMD(0xA8); /*multiplex ratio*/
    OLED_WR_CMD(0x3F); /*duty = 1/64*/
    OLED_WR_CMD(0xad); /*set charge pump enable*/
    OLED_WR_CMD(0x8b); /* 0x8B 内供 VCC */
    OLED_WR_CMD(0x33); /*0X30---0X33 set VPP 9V */
    OLED_WR_CMD(0xC8); /*Com scan direction*/
    OLED_WR_CMD(0xD3); /*set display offset*/
    OLED_WR_CMD(0x00); /* 0x20 */
    OLED_WR_CMD(0xD5); /*set osc division*/
    OLED_WR_CMD(0x80);
    OLED_WR_CMD(0xD9); /*set pre-charge period*/
    OLED_WR_CMD(0x1f); /*0x22*/
    OLED_WR_CMD(0xDA); /*set COM pins*/
    OLED_WR_CMD(0x12);
    OLED_WR_CMD(0xdb); /*set vcomh*/
    OLED_WR_CMD(0x40);
    OLED_Clear();
    OLED_WR_CMD(0xAF); /*display ON*/

}


int main(void)
{
    Executor exec = Executor();
    exec.init();
  
    int i;

    button_setup();
    gpio_setup();

    spi_setup();
    OLED_Init();
    

    // gpio_clear(GPIOA, GPIO4);
    // spi_blocking_send(SPI1, 0xFF);
    // gpio_set(GPIOA, GPIO4);

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
