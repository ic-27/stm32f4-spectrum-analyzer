#include <cstdint>
#include "oled.h"

#ifndef CPPUTEST_TEST
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

/**
 * Oled::init() - Initialize OLED driver
 *
 * See p16 https://www.smart-prototyping.com/image/data/2020/11/102104%201.3%20inch%20OLED%20Display/Datasheet.pdf
 *
 * Return: void
 */
void Oled::init(void)
{
    this->init_gpio();
    this->init_spi();

    // as instructed by datasheet
    gpio_clear(GPIOA, OLED_RST_PIN);
    // delay for ~200 ms
    
    #warning use a timer for this
    for (uint32_t i=0; i<2400000; ++i) {__asm__("nop");}
    gpio_set(GPIOA, OLED_RST_PIN);

    this->wr_cmd(0xAE); /*display off*/
    this->wr_cmd(0x02); /*set lower column address*/
    this->wr_cmd(0x10); /*set higher column address*/
    this->wr_cmd(0x40); /*set display start line*/
    this->wr_cmd(0xB0); /*set page address*/
    this->wr_cmd(0x81); /*contract control*/
    this->wr_cmd(0xcf); /*128*/
    this->wr_cmd(0xA1); /*set segment remap*/
    this->wr_cmd(0xA6); /*normal / reverse*/
    this->wr_cmd(0xA8); /*multiplex ratio*/
    this->wr_cmd(0x3F); /*duty = 1/64*/
    this->wr_cmd(0xad); /*set charge pump enable*/
    this->wr_cmd(0x8b); /* 0x8B 内供 VCC */
    this->wr_cmd(0x33); /*0X30---0X33 set VPP 9V */
    this->wr_cmd(0xC8); /*Com scan direction*/
    this->wr_cmd(0xD3); /*set display offset*/
    this->wr_cmd(0x00); /* 0x20 */
    this->wr_cmd(0xD5); /*set osc division*/
    this->wr_cmd(0x80);
    this->wr_cmd(0xD9); /*set pre-charge period*/
    this->wr_cmd(0x1f); /*0x22*/
    this->wr_cmd(0xDA); /*set COM pins*/
    this->wr_cmd(0x12);
    this->wr_cmd(0xdb); /*set vcomh*/
    this->wr_cmd(0x40);
    this->clear();
    this->wr_cmd(0xAF); /*display ON*/
}

/**
 * init_gpio() - Initialize gpio pins for oled
 * 
 * Return: void
 */
void inline Oled::init_gpio(void)
{
    // data/control pin
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OLED_DC_PIN);
    gpio_clear(GPIOA, OLED_DC_PIN); // init to cmd mode

    // reset pin
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OLED_RST_PIN);
}

/**
 * init_spi() - Initialize spi for oled
 * 
 * Return: void
 */
void inline Oled::init_spi(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
		    OLED_CLK_PIN | OLED_MISO_PIN | OLED_MOSI_PIN);
    gpio_set_af(GPIOA, GPIO_AF5, OLED_CLK_PIN | OLED_MISO_PIN | OLED_MOSI_PIN);

    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OLED_SS_PIN); // ss
    gpio_set(GPIOA, OLED_SS_PIN);

    rcc_periph_clock_enable(RCC_SPI1);
    spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_32,
		    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
		    SPI_CR1_CPHA_CLK_TRANSITION_1,
		    SPI_CR1_DFF_8BIT,
		    SPI_CR1_MSBFIRST);
    
    spi_enable_ss_output(SPI1);
    spi_enable(SPI1);
}

/**
 * Oled::wr_cmd() - Write a command to the SH1106
 * @data: data to write
 *
 * Return: void
 */
void Oled::wr_cmd(uint8_t data)
{
    gpio_clear(GPIOA, OLED_DC_PIN); // set to cmd mode

    gpio_clear(GPIOA, OLED_SS_PIN);
    spi_blocking_send(SPI1, data);
    gpio_set(GPIOA, OLED_SS_PIN);
}

/**
 * Oled::wr_data() - Write pixel data to the SH1106
 * @data: data to write
 *
 * Return: void
 */
void Oled::wr_data(uint8_t data)
{
    gpio_set(GPIOA, OLED_DC_PIN); // set to data mode

    gpio_clear(GPIOA, OLED_SS_PIN);
    spi_blocking_send(SPI1, data);
    gpio_set(GPIOA, OLED_SS_PIN);
}

#endif

/**
 * Oled::clear - Clear OLED screen
 * 
 * Return: void
 */
void Oled::clear(void)
{
    for (uint8_t j=OLED_PAGE_START; j<=OLED_PAGE_END; ++j) {
	this->wr_cmd(j); // set page (shift to another column)
	this->wr_cmd(0x02); // set lower column addr
	this->wr_cmd(0x10); // set higher column addr
	for (uint8_t k=0; k<OLED_ROWS; ++k) {
	    this->wr_data(0x00);
	}
    }
}

/**
 * Oled::update - Clear OLED screen
 * 
 * Return: void
 */
void Oled::update(uint8_t *internalOledArray)
{
    for (uint8_t i=0, j=OLED_PAGE_START; j<=OLED_PAGE_END; ++i, ++j) {
	this->wr_cmd(j); // set page
	this->wr_cmd(0x02); // set lower column addr
	this->wr_cmd(0x10); // set higher column addr
	
	uint8_t k=0;
	for (; k<internalOledArray[i]; ++k) { // internalOledArray[i] always < OLED_ROWS
	    this->wr_data(0xFF);
	}
	for (; k<OLED_ROWS; ++k) {
	    this->wr_data(0x00);
	}
    }
}
