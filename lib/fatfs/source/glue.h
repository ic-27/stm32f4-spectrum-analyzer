#ifndef GLUE_H
#define GLUE_H

#include "ff.h"

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

#define SD_CLK_PIN	GPIO13
#define SD_MISO_PIN	GPIO14
#define SD_MOSI_PIN	GPIO15
#define SD_SS_PIN	GPIO12

#define CMD17_TOKEN	0xFE

#define DUMMY_VALUE 0xFF
#define RETRY_TIMES 0xFF
#define GLUE_SENDCMD_RETARR_MAXLEN 5

EXTERNC void glue_initSPI(void); // static
EXTERNC DINITRESULT glue_initCard(void);
EXTERNC int32_t glue_sendCmd(uint8_t *retArr, uint8_t cmd, uint32_t arg, uint8_t crc);
EXTERNC int32_t glue_rxDataBlock(BYTE *buff, UINT len);



#endif

