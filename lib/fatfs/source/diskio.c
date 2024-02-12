/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

/* Definitions of physical drive number for each drive (unused) */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

static DINITRESULT disk_initialized = DINIT_NOTRDY;

static void init_spi(void);
int32_t send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc);
int32_t rx_data_block(BYTE *buff, UINT len);
static uint8_t initialize_card(void);

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(
		    __attribute__((unused))BYTE pdrv        /* Physical drive number to identify the drive */
		    )
{
    if (DINIT_OK == disk_initialized) {
	return RES_OK;
    } else {
	return RES_NOTRDY;
    }
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(
			__attribute__((unused))BYTE pdrv        /* Physical drive nmuber to identify the drive */
			)
{
    disk_initialized = initialize_card();
    return disk_status(DUMMY_BYTE);
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(
		  BYTE pdrv,        /* Physical drive nmuber to identify the drive */
		  BYTE *buff,		/* Data buffer to store read data */
		  LBA_t sector,	/* Start sector in LBA */
		  UINT count		/* Number of sectors to read */
		  )
{
    if (disk_initialized != DINIT_OK) {
	return RES_NOTRDY;
    }

    if (1 == count) { // single read
	send_cmd(17, sector, 0);
	rx_data_block(buff, 512);
    } else { // multiple read
	send_cmd(18, sector, 0);
	do {
	    rx_data_block(buff, 512);
	    buff += 512;
	} while (--count);
    }

    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
		    BYTE pdrv,			/* Physical drive nmuber to identify the drive */
		    const BYTE *buff,	/* Data to be written */
		    LBA_t sector,		/* Start sector in LBA */
		    UINT count			/* Number of sectors to write */
		    )
{
    DRESULT res;
    int result;

    switch (pdrv) {
    case DEV_RAM :
	// translate the arguments here

	result = RAM_disk_write(buff, sector, count);

	// translate the reslut code here

	return res;

    case DEV_MMC :
	// translate the arguments here

	result = MMC_disk_write(buff, sector, count);

	// translate the reslut code here

	return res;

    case DEV_USB :
	// translate the arguments here

	result = USB_disk_write(buff, sector, count);

	// translate the reslut code here

	return res;
    }

    return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
		    BYTE pdrv,		/* Physical drive nmuber (0..) */
		    BYTE cmd,		/* Control code */
		    void *buff		/* Buffer to send/receive control data */
		    )
{
    DRESULT res;
    int result;

    switch (pdrv) {
    case DEV_RAM :

	// Process of the command for the RAM drive

	return res;

    case DEV_MMC :

	// Process of the command for the MMC/SD card

	return res;

    case DEV_USB :

	// Process of the command the USB drive

	return res;
    }

    return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Glue Functions                                                        */
/*-----------------------------------------------------------------------*/

/**
 * init_spi() - Initialize spi for microSD card adapater
 *
 * Return: void
 */
static void init_spi(void)
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

int32_t send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint32_t ret;

    spi_xfer(SPI2, 0xFF);
    spi_xfer(SPI2, 0xFF);

    gpio_clear(GPIOB, SD_SS_PIN);
    
    spi_xfer(SPI2, cmd | 0x40);
    spi_xfer(SPI2, arg >> 24);
    spi_xfer(SPI2, arg >> 16);
    spi_xfer(SPI2, arg >> 8);
    spi_xfer(SPI2, arg >> 0);
    spi_xfer(SPI2, crc);

    for (uint8_t retry=0; retry < 200; ++retry) {
	ret = spi_xfer(SPI2, 0xFF);
	if (!(ret & 0x80)) { // got a valid response, keep reading
	    while (spi_xfer(SPI2, 0xFF) != 0xFF);
	    goto send_cmd_end;
	}
    }
    ret = -1;
 send_cmd_end:
    gpio_set(GPIOB, SD_SS_PIN);
    spi_xfer(SPI2, 0xFF);
    
    return ret;
}

int32_t rx_data_block(BYTE *buff, UINT len)
{
    gpio_clear(GPIOB, SD_SS_PIN);

    #warning add a timeout later
    uint8_t token;
    do {
	token = spi_xfer(SPI2, DUMMY_BYTE);
    } while (CMD17_TOKEN != token);

    for (UINT i=0; i<len; ++i) {
	buff[i] = spi_xfer(SPI2, DUMMY_BYTE);
    }

    gpio_set(GPIOB, SD_SS_PIN);
    return 0;
}

/**
 * initialize_card()
 *
 * Make sure SPI is initialized before calling this function.
 *
 * Return: void
 */
DINITRESULT initialize_card(void)
{
    init_spi();
    
#warning give proper delay
    // wait >= 1 ms
    for(int i=0; i<300000; ++i); // delay

    // apply 74+ dummy clock pulses enter native operating mode
    gpio_set(GPIOB, SD_SS_PIN);
    for (uint8_t i=0; i<10; ++i) {
	spi_xfer(SPI2, 0xFF);
    }
    
    // software reset
    send_cmd(0, 0, 0x95);
    
    /* // detect SDC v1 or SDC v2 (we are SDC v2) */
    send_cmd(8, 0x01AA, 0x87);

    // loop may take up to 1000ms
    // takes around ~16ms for me
#warning add a timer to limit to 1000ms
    uint32_t ret;
    send_cmd(55, 0, 0);
    ret = send_cmd(41, 0x40000000, 0);
    while(ret != 0x00) {
	send_cmd(55, 0, 0);
	ret = send_cmd(41, (1<<30), 0);
    }

    // check CCS bit in OCR (if set it is high-capacity card)
    send_cmd(58, 0, 0);
    
    // set block size
    send_cmd(16, 512, 0);

    return DINIT_OK;
}
