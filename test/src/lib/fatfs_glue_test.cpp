#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "glue.h"

TEST_GROUP(FatFS)
{
    void setup()
    {
    }
    
    void teardown()
    {
	mock().checkExpectations();
	mock().clear();
    }
};

// mock functions: start
uint16_t spi_xfer(uint32_t spi, uint16_t data)
{
    return mock().actualCall(__func__)
	.withParameter("spi", spi)
	.withParameter("data", data)
	.returnIntValue();
}

void gpio_set(uint32_t gpioport, uint16_t gpios)
{
    mock().actualCall(__func__)
	.withParameter("gpioport", gpioport)
	.withParameter("gpios", gpios);
}

void gpio_clear(uint32_t gpioport, uint16_t gpios)
{
    mock().actualCall(__func__)
	.withParameter("gpioport", gpioport)
	.withParameter("gpios", gpios);
}
// mock functions: end

// help functions: start
int32_t glue_sendCmd_TransmitArgsHelper(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", cmd | 0x40)
	.andReturnValue(DUMMY_VALUE);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", arg >> 24)
	.andReturnValue(DUMMY_VALUE);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", arg >> 16)
	.andReturnValue(DUMMY_VALUE);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", arg >> 8)
	.andReturnValue(DUMMY_VALUE);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", arg >> 0)
	.andReturnValue(DUMMY_VALUE);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", crc)
	.andReturnValue(DUMMY_VALUE);
}

int32_t glue_sendCmd_StandardExitHelper(void)
{
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0x00);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);
}
// helper functions: end

// test glue_sendCmd: start
TEST(FatFS, glue_sendCmd_ReturnFailureOnTimeoutFailure)
{
    // setup
    uint32_t ret;
    uint8_t dummy_cmd = 0;
    uint32_t dummy_arg = 0;
    uint8_t dummy_crc = 0;

    mock().expectNCalls(2, "spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);

    mock().expectOneCall("gpio_clear").ignoreOtherParameters();
    
    glue_sendCmd_TransmitArgsHelper(dummy_cmd, dummy_arg, dummy_crc);

    // Keep transmitting until tried RETRY_TIMES
    mock().expectNCalls(RETRY_TIMES, "spi_xfer") 
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0xFF);

    mock().expectOneCall("gpio_set").ignoreOtherParameters();
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);
    
    // exercise
    ret = glue_sendCmd(NULL, dummy_cmd, dummy_arg, dummy_crc);
    
    // verify
    CHECK_EQUAL(-1, ret);
}

TEST(FatFS, glue_sendCmd_ReturnsSuccessOn0x1RetVal)
{
    // setup
    uint32_t ret;
    uint8_t ret_arr[GLUE_SENDCMD_RETARR_MAXLEN] = {0};
    uint8_t dummy_cmd = 0;
    uint32_t dummy_arg = 0;
    uint8_t dummy_crc = 0;

    mock().expectNCalls(2, "spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);

    mock().expectOneCall("gpio_clear").ignoreOtherParameters();
    
    glue_sendCmd_TransmitArgsHelper(dummy_cmd, dummy_arg, dummy_crc);

    // Got 0x1 value and then no more after that (0xFF signals end)
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0x01);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0xFF);

    mock().expectOneCall("gpio_set").ignoreOtherParameters();
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);
    
    // exercise
    ret = glue_sendCmd(ret_arr, dummy_cmd, dummy_arg, dummy_crc);
    
    // verify
    CHECK_EQUAL(0, ret);
    CHECK_EQUAL(1, ret_arr[0]);
}

TEST(FatFS, glue_sendCmd_ReturnsSuccessOn0x0RetVal)
{
    // setup
    uint32_t ret;
    uint8_t ret_arr[GLUE_SENDCMD_RETARR_MAXLEN] = {0};
    uint8_t dummy_cmd = 0;
    uint32_t dummy_arg = 0;
    uint8_t dummy_crc = 0;
    
    mock().expectNCalls(2, "spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);

    mock().expectOneCall("gpio_clear").ignoreOtherParameters();
    
    glue_sendCmd_TransmitArgsHelper(dummy_cmd, dummy_arg, dummy_crc);

    // Got 0x0 value and then no more after that (0xFF signals end)
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0x00);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0xFF);

    mock().expectOneCall("gpio_set").ignoreOtherParameters();
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);
    
    // exercise
    ret = glue_sendCmd(ret_arr, dummy_cmd, dummy_arg, dummy_crc);
    
    // verify
    CHECK_EQUAL(0, ret);
    CHECK_EQUAL(0, ret_arr[0]);
}

// // // // // https://stackoverflow.com/questions/2365897/initializing-sd-card-in-spi-issues
TEST(FatFS, glue_sendCmd_SendsExtraClockSignalsToPrepareSDCard)
{
    // setup
    uint8_t ret_arr[GLUE_SENDCMD_RETARR_MAXLEN] = {0};
    uint8_t dummy_cmd = 0;
    uint32_t dummy_arg = 0;
    uint8_t dummy_crc = 0;

    // send extra clock signals. some sd cards need require this to work
    for (uint8_t i=0; i<2; ++i) {
	mock().expectOneCall("spi_xfer")
	    .withParameter("spi", SPI2)
	    .withParameter("data", DUMMY_VALUE)
	    .andReturnValue(DUMMY_VALUE);
    }

    mock().expectOneCall("gpio_clear").ignoreOtherParameters();
    
    glue_sendCmd_TransmitArgsHelper(dummy_cmd, dummy_arg, dummy_crc);

    glue_sendCmd_StandardExitHelper();

    mock().expectOneCall("gpio_set").ignoreOtherParameters();
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);
    
    // exercise
    glue_sendCmd(ret_arr, dummy_cmd, dummy_arg, dummy_crc);
    
    // verify
    // don't care about ret value, just care about whether mock functions were called
}

TEST(FatFS, glue_sendCmd_TransmitsCmdArgsCrc)
{
    // setup
    uint8_t ret_arr[GLUE_SENDCMD_RETARR_MAXLEN] = {0};
    uint8_t dummy_cmd = 0;
    uint32_t dummy_arg = 0;
    uint8_t dummy_crc = 0;

    mock().expectNCalls(2, "spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);

    mock().expectOneCall("gpio_clear").ignoreOtherParameters();
    
    // transmit cmd args
    glue_sendCmd_TransmitArgsHelper(dummy_cmd, dummy_arg, dummy_crc);

    glue_sendCmd_StandardExitHelper();

    mock().expectOneCall("gpio_set").ignoreOtherParameters();
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);
    
    // exercise
    glue_sendCmd(ret_arr, dummy_cmd, dummy_arg, dummy_crc);
    
    // verify
    // don't care about ret
}

TEST(FatFS, glue_sendCmd_ReturnsDataInArray)
{
    // setup
    uint32_t ret;
    uint8_t ret_arr[GLUE_SENDCMD_RETARR_MAXLEN] = {0};
    uint8_t dummy_cmd = 0;
    uint32_t dummy_arg = 0;
    uint8_t dummy_crc = 0;

    for (uint8_t i=0; i<2; ++i) {
	mock().expectOneCall("spi_xfer")
	    .withParameter("spi", SPI2)
	    .withParameter("data", DUMMY_VALUE)
	    .andReturnValue(DUMMY_VALUE);
    }

    mock().expectOneCall("gpio_clear").ignoreOtherParameters();
    
    // transmit cmd args
    glue_sendCmd_TransmitArgsHelper(dummy_cmd, dummy_arg, dummy_crc);

    // as long as we don't receive 0xFF or end of array, should keep storing into passed in array
    mock().expectNCalls(3, "spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0x00);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0x01);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0xCC);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0xFF);

    mock().expectOneCall("gpio_set").ignoreOtherParameters();
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);
    
    // exercise
    ret = glue_sendCmd(ret_arr, dummy_cmd, dummy_arg, dummy_crc);
    
    // verify
    CHECK_EQUAL(0, ret);
    CHECK_EQUAL(0, ret_arr[0]);
    CHECK_EQUAL(0, ret_arr[1]);
    CHECK_EQUAL(0, ret_arr[2]);
    CHECK_EQUAL(0x01, ret_arr[3]);
    CHECK_EQUAL(0xCC, ret_arr[4]);
}

TEST(FatFS, glue_sendCmd_ReturnsInfoInArrayAndChecksForOverflow)
{
    // setup
    uint32_t ret;
    uint8_t ret_arr[GLUE_SENDCMD_RETARR_MAXLEN] = {0};
    uint8_t dummy_cmd = 0;
    uint32_t dummy_arg = 0;
    uint8_t dummy_crc = 0;

    for (uint8_t i=0; i<2; ++i) {
	mock().expectOneCall("spi_xfer")
	    .withParameter("spi", SPI2)
	    .withParameter("data", DUMMY_VALUE)
	    .andReturnValue(DUMMY_VALUE);
    }

    mock().expectOneCall("gpio_clear").ignoreOtherParameters();
    
    // transmit cmd args
    glue_sendCmd_TransmitArgsHelper(dummy_cmd, dummy_arg, dummy_crc);

#warning future-proof this test by keep tracking of what count we are at
    mock().expectNCalls(3, "spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0x00);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0x01);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0xCC);
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(0xAA); // terminated based on length
    

    mock().expectOneCall("gpio_set").ignoreOtherParameters();
    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);
    
    // exercise
    ret = glue_sendCmd(ret_arr, dummy_cmd, dummy_arg, dummy_crc);
        
    // verify
    CHECK_EQUAL(0, ret);
    CHECK_EQUAL(0, ret_arr[0]);
    CHECK_EQUAL(0, ret_arr[1]);
    CHECK_EQUAL(0, ret_arr[2]);
    CHECK_EQUAL(0x01, ret_arr[3]);
    CHECK_EQUAL(0xCC, ret_arr[4]);
}

TEST(FatFS, glue_sendCmd_TogglesSlaveSelect)
{
    uint32_t ret;
    uint8_t ret_arr[GLUE_SENDCMD_RETARR_MAXLEN] = {0};
    uint8_t dummy_cmd = 0;
    uint32_t dummy_arg = 0;
    uint8_t dummy_crc = 0;

    // setup
    mock().expectOneCall("gpio_clear") // clear SS
	.withParameter("gpioport", GPIOB)
	.withParameter("gpios", SD_SS_PIN);
    
    mock().expectNCalls(2, "spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);
    glue_sendCmd_TransmitArgsHelper(dummy_cmd, dummy_arg, dummy_crc);
    
    glue_sendCmd_StandardExitHelper();

    mock().expectOneCall("gpio_set") // set SS
	.withParameter("gpioport", GPIOB)
	.withParameter("gpios", SD_SS_PIN);

    mock().expectOneCall("spi_xfer")
	.withParameter("spi", SPI2)
	.withParameter("data", DUMMY_VALUE)
	.andReturnValue(DUMMY_VALUE);
    
    // exercise
    ret = glue_sendCmd(ret_arr, dummy_cmd, dummy_arg, dummy_crc);
    
    // verify
}
// test glue_sendCmd: end

