#include "display.h"

Display::Display(void)
{
    this->init();
}

void Display::init(void)
{
    Oled::init();
}

void Display::update(uint8_t *internalOledArray)
{
    for (uint8_t i=0, j=OLED_PAGE_START; j<=OLED_PAGE_END; ++i, ++j) {
	Oled::wr_cmd(j); // set page
	Oled::wr_cmd(0x02); // set lower column addr
	Oled::wr_cmd(0x10); // set higher column addr
	
	uint8_t k=0;
	for (; k<internalOledArray[i]; ++k) { // internalOledArray[i] always < OLED_ROWS
	    Oled::wr_data(0xFF);
	}
	for (; k<OLED_ROWS; ++k) {
	    Oled::wr_data(0x00);
	}
    }
}

