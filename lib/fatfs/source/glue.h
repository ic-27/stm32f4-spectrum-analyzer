#ifndef GLUE_H
#define GLUE_H

#include "ff.h"
#include "diskio.h"

// defs & defines specifically used for cpputest
#ifdef CPPUTEST_TEST
#define SPI2	0		// val doesn't matter
#define GPIOB	0		// val doesn't matter
#define GPIO12	0		// val doesn't matter
uint16_t spi_xfer(uint32_t spi, uint16_t data);
void gpio_set(uint32_t gpioport, uint16_t gpios);
void gpio_clear(uint32_t gpioport, uint16_t gpios);
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

typedef enum {
    DINIT_OK = 0,
    DINIT_NOTRDY
} DINITRESULT;

// pin defines
#define SD_CLK_PIN	GPIO13
#define SD_MISO_PIN	GPIO14
#define SD_MOSI_PIN	GPIO15
#define SD_SS_PIN	GPIO12

// 
#define DUMMY_VALUE			0xFF
#define RETRY_TIMES			0xFF
#define GLUE_SENDCMD_RETARR_MAXLEN	5
#define DUMMY_CLK_PULSES		10	// 10*8 = 80 > 74 clock pulses

// sd card specific commands, expected values
#define SOFTWARE_RST_CMD	0
#define CHK_VER_CMD		8
#define SET_BLK_CMD		16
#define INIT_CMD		41
#define APP_CMD			55
#define CHK_CAP_CMD		58 // check if high-capacity card

#define INIT_STATE	0x00
#define IDLE_STATE	0x01
#define CMD171824_TOKEN	0xFE
#define BLK_SIZE	512

EXTERNC DSTATUS glue_diskStatus(void); // status
EXTERNC DINITRESULT glue_initCard(void);
EXTERNC int32_t glue_sendCmd(uint8_t *retArr, uint8_t cmd, uint32_t arg, uint8_t crc);
EXTERNC DRESULT glue_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count);
EXTERNC int32_t glue_rxDataBlock(BYTE *buff, UINT len);



#endif

