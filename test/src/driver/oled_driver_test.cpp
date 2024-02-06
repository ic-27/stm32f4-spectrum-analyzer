#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <cstdint>
#include "oled.h"

uint8_t g_oledArray[OLED_COLS][OLED_ROWS] = {1};

TEST_GROUP(OledDriver)
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

static uint8_t row = -8;
/**
 * Oled::wr_cmd() - Test implementation of wr_cmd to mock what actually happens
 * when writing a cmd to OLED
 *
 * Return: void
 */
void Oled::wr_cmd(uint8_t data)
{
    if (data >= OLED_PAGE_START && data <= OLED_PAGE_END) {
	// a single page is a 128x8 block of memory
	// each time called, we update to a new page
	// in this case, it will just be the next row, so +8
	row += 8;
    }
}

/**
 * Oled::wr_data() - Test implementation of wr_data to mock what actually happens
 * when writing data to OLED
 *
 * Return: void
 */
void Oled::wr_data(uint8_t data)
{
    // each time we call this func, the oled stores the bits of data across, but
    // vertically. so go increment by row
    for (uint8_t col=0; col<OLED_COLS; ++col) {
	for(uint8_t r=row; r<row+8; ++r) {
	    #warning change so that an element in the 2D array represents only a single bit. current implementation only works for clear
	    g_oledArray[col][r] = data;
	}
    }
}
 
TEST(OledDriver, OledClearClearsAllPixels)
{
    // instead of writing to an actually oled, we write to g_oledArray for
    // testing. it is initialized to all 1's
    
    // setup
    Oled oled = Oled();

    // exercise
    oled.clear();

    // verify 
    for (uint8_t i=0; i<OLED_COLS; ++i) {
	for (uint8_t j=0; j<OLED_ROWS; ++j) {
	    CHECK_EQUAL(0, g_oledArray[i][j]);
	}
    }
}
