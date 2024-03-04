#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <cstdint>
#include "oled.h"

#define START_COL -8
#define START_ROW 0

static uint8_t col = START_COL;
static uint8_t row = START_ROW;
static uint8_t g_oledArray[OLED_COLS][OLED_ROWS] = {1};

TEST_GROUP(Display)
{
    void setup()
    {
	col = START_COL;
	row = START_ROW;
    }
    
    void teardown()
    {
	mock().checkExpectations();
	mock().clear();
    }
};

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
	// in this case, it will just be the next col, so +8
	
	col += 8; // each col represents 8 bits of data
	row = 0;  // reset the row for simulation
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
    // each time we call this func, oled stores the bits of data across
    for (uint8_t c=col; c<col+8; ++c) {
	g_oledArray[c][row] = data;
    }
    ++row; // the oled automatically increments the row. since we are simulating, we do it manually.
}
 
TEST(Display, OledClearClearsAllPixels)
{
    // instead of writing to an actually oled, we write to g_oledArray for
    // testing. it is initialized to all 1's
    
    // setup
    for (uint8_t i=0; i<OLED_COLS; ++i) {
	for (uint8_t j=0; j<OLED_ROWS; ++j) {
	    g_oledArray[i][j] = 1;
	}
    }
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

TEST(Display, OledCorrectlyUpdatesZero)
{
    // instead of writing to an actually oled, we write to g_oledArray for
    // testing. it is initialized to all 1's
    
    // setup
    for (uint8_t i=0; i<OLED_COLS; ++i) { // oled externa display output
	for (uint8_t j=0; j<OLED_ROWS; ++j) {
	    g_oledArray[i][j] = 1;
	}
    }

    uint8_t g_InternalOledArray[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // internal array representation
    Oled oled = Oled();

    // exercise
    oled.update(g_InternalOledArray);

    // verify 
    for (uint8_t i=0; i<OLED_COLS; ++i) {
	for (uint8_t j=0; j<OLED_ROWS; ++j) {
	    CHECK_EQUAL(0, g_oledArray[i][j]);
	}
    }
}

TEST(Display, OledCorrectlyUpdatesOne)
{
    // setup
    for (uint8_t i=0; i<OLED_COLS; ++i) { // oled externa display output
	for (uint8_t j=0; j<OLED_ROWS; ++j) {
	    g_oledArray[i][j] = 0;
	}
    }

    uint8_t g_InternalOledArray[8] = {1, 1, 1, 1, 1, 1, 1, 1}; // internal array representation
    Oled oled = Oled();

    // exercise
    oled.update(g_InternalOledArray);

    // verify 
    for (uint8_t i=0; i<OLED_COLS; ++i) {
	for (uint8_t j=0; j<OLED_ROWS; ++j) {
	    if (0==j) {
		// first row should be 1s. Actual implementation writes 0xFF to fill all 8 bits.
		// so just check for that
		CHECK_EQUAL(0xFF, g_oledArray[i][j]);
	    } else {
		CHECK_EQUAL(0, g_oledArray[i][j]);
	    }
	}
    }
}

TEST(Display, OledCorrectlyUpdates30)
{
    // setup
    for (uint8_t i=0; i<OLED_COLS; ++i) { // oled externa display output
	for (uint8_t j=0; j<OLED_ROWS; ++j) {
	    g_oledArray[i][j] = 0;
	}
    }

    uint8_t g_InternalOledArray[8] = {30, 30, 30, 30, 30, 30, 30, 30}; // internal array representation
    Oled oled = Oled();

    // exercise
    oled.update(g_InternalOledArray);

    // verify 
    for (uint8_t i=0; i<OLED_COLS; ++i) {
	for (uint8_t j=0; j<OLED_ROWS; ++j) {
	    if (j<30) {
		// first 30 rows should be 1s. Actual implementation writes 0xFF to fill all 8 bits.
		// so just check for that
		CHECK_EQUAL(0xFF, g_oledArray[i][j]);
	    } else {
		CHECK_EQUAL(0, g_oledArray[i][j]);
	    }
	}
    }
}
