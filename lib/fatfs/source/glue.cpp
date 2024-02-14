#include "glue.h"
#include "diskio.h"

#ifndef CPPUTEST_TEST
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

/**
 * glue_initSPI() - Initialize spi for microSD card adapater
 *
 * No need to perform unit testing on this, libraries are assumed to be trusted.
 *
 * Return: void
 */
void glue_initSPI(void) // static
{
    rcc_periph_clock_enable(RCC_GPIOB);

    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
		    SD_CLK_PIN | SD_MOSI_PIN);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SD_MISO_PIN);
		    
    gpio_set_af(GPIOB, GPIO_AF5, SD_CLK_PIN | SD_MISO_PIN | SD_MOSI_PIN);

    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SD_SS_PIN);
    gpio_set(GPIOB, SD_SS_PIN);

    rcc_periph_clock_enable(RCC_SPI2);
    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_128,
		    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
		    SPI_CR1_CPHA_CLK_TRANSITION_1,
		    SPI_CR1_DFF_8BIT,
		    SPI_CR1_MSBFIRST);
    
    spi_enable_ss_output(SPI2);
    spi_enable(SPI2);
}
#endif

/**
 * glue_initCard()
 *
 * 
 *
 * Return: Enum whether initialization was successful or not
 */
DINITRESULT glue_initCard(void)
{
    
    return DINIT_OK;
}

int32_t glue_sendCmd(uint8_t *retArr, uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint32_t retVal;
    
    spi_xfer(SPI2, DUMMY_VALUE);
    spi_xfer(SPI2, DUMMY_VALUE);

    gpio_clear(GPIOB, SD_SS_PIN);
    
    spi_xfer(SPI2, cmd | 0x40);
    spi_xfer(SPI2, arg >> 24);
    spi_xfer(SPI2, arg >> 16);
    spi_xfer(SPI2, arg >> 8);
    spi_xfer(SPI2, arg >> 0);
    spi_xfer(SPI2, crc);

    for (uint8_t retry=0; retry<RETRY_TIMES; ++retry) {
	retVal = spi_xfer(SPI2, DUMMY_VALUE);
	if (!(retVal & 0x80)) { // got a valid response, keep reading
	    uint8_t count = 0;
	    retArr[count++] = retVal;
	    retVal = spi_xfer(SPI2, DUMMY_VALUE);
	    while ((count < GLUE_SENDCMD_RETARR_MAXLEN) && (retVal != DUMMY_VALUE)) {
		retArr[count++] = retVal;
		retVal = spi_xfer(SPI2, DUMMY_VALUE);
	    }
	    retVal = 0;
	    goto glue_sendCmd_end;
	}
    }
    retVal = -1;
 glue_sendCmd_end:
    gpio_set(GPIOB, SD_SS_PIN);
    
    return retVal;
}

int32_t glue_rxDataBlock(BYTE *buff, UINT len)
{
    return 0;
}


