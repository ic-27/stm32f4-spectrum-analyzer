#include "glue.h"
#include "diskio.h"

#ifndef CPPUTEST_TEST
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

DINITRESULT diskInitialized = DINIT_NOTRDY;

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

DSTATUS glue_diskStatus(void)
{
    if (DINIT_OK == diskInitialized) {
	return RES_OK;
    } else {
	return RES_NOTRDY;
    }
}


/**
 * glue_initCard() - Initialize card
 *
 * Return: Enum whether initialization was successful or not
 */
DINITRESULT glue_initCard(void)
{
    uint8_t retArr[GLUE_SENDCMD_RETARR_MAXLEN] = {0};
    glue_initSPI();
    
    #warning give proper delay
    for(int i=0; i<300000; ++i); // wait >= 1ms

    // apply 74+ dummy clock pulses enter native operating mode
    gpio_set(GPIOB, SD_SS_PIN);
    for (uint8_t i=0; i<DUMMY_CLK_PULSES; ++i) {
	spi_xfer(SPI2, 0xFF);
    }
    
    if (glue_sendCmd(retArr, SOFTWARE_RST_CMD, 0, 0x95)
	|| retArr[0] != IDLE_STATE) {
	goto glue_initCardFail;
    }
    if (glue_sendCmd(retArr, CHK_VER_CMD, 0x1AA, 0x87)
	|| retArr[0] != IDLE_STATE
	|| retArr[3] != 0x01 || retArr[4] != 0xAA) {
	goto glue_initCardFail;
    }

    #warning add a timer to limit to 1000ms
    // can take up to 1000ms
    if (glue_sendCmd(retArr, APP_CMD, 0, 0)) goto glue_initCardFail;
    if (glue_sendCmd(retArr, INIT_CMD, 0x40000000, 0)) goto glue_initCardFail;
    while (retArr[0] != INIT_STATE) {
	if (glue_sendCmd(retArr, APP_CMD, 0, 0)) goto glue_initCardFail;
	if (glue_sendCmd(retArr, INIT_CMD, 0x40000000, 0)) goto glue_initCardFail;
    }

    if (glue_sendCmd(retArr, CHK_CAP_CMD, 0, 0)
	|| retArr[0] != INIT_STATE
	|| retArr[1] != 0xC0) {
	goto glue_initCardFail;
    }

    if (glue_sendCmd(retArr, SET_BLK_CMD, BLK_SIZE, 0)
	|| retArr[0] != INIT_STATE) {
	goto glue_initCardFail;
    }

    // spi_set_baudrate_prescaler doesn't work properly for some reason
    // 10.5 MHz seems to be fastest this board can work at w/ microsd
    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_4,
		    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
		    SPI_CR1_CPHA_CLK_TRANSITION_1,
		    SPI_CR1_DFF_8BIT,
		    SPI_CR1_MSBFIRST);
    
    return DINIT_OK;
 glue_initCardFail:
    return DINIT_NOTRDY;
}

DRESULT glue_read(
	       BYTE pdrv,        /* Physical drive nmuber to identify the drive */
	       BYTE *buff,		/* Data buffer to store read data */
	       LBA_t sector,	/* Start sector in LBA */
	       UINT count		/* Number of sectors to read */
	       )
{
    if (DINIT_OK != diskInitialized) {
	return RES_NOTRDY;
    }
    
    uint8_t retArr[GLUE_SENDCMD_RETARR_MAXLEN] = {0};

#warning handle glue_sendcmd errors
    if (1 == count) { // single read
	glue_sendCmd(retArr, 17, sector, 0);
	glue_rxDataBlock(buff, 512);
    } else { // multiple read
	glue_sendCmd(retArr, 18, sector, 0);
	do {
	    glue_rxDataBlock(buff, 512);
	    buff += 512;
	} while (--count);
    }
    return RES_OK;
}

int32_t glue_rxDataBlock(BYTE *buff, UINT len)
{
    gpio_clear(GPIOB, SD_SS_PIN);

#warning glue_rxDataBlock: add a timeout later
    uint8_t token;
    do {
	token = spi_xfer(SPI2, DUMMY_BYTE);
    } while (CMD171824_TOKEN != token);

    for (UINT i=0; i<len; ++i) {
	buff[i] = spi_xfer(SPI2, DUMMY_BYTE);
    }
    
    gpio_set(GPIOB, SD_SS_PIN);
    return 0;
}

#endif

/**
 * glue_sendCmd()
 * @retArr: Array to store return values in
 * @cmd: command to send
 * @arg: arguments to send
 * @crc: cyclic redundancy check
 */
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
    spi_xfer(SPI2, DUMMY_VALUE);
    
    return retVal;
}



