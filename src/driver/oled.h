#ifndef OLED_H
#define OLED_H

#include <cstdint>

#define OLED_DC_PIN	GPIO0
#define OLED_RST_PIN	GPIO2
#define OLED_SS_PIN	GPIO4
#define OLED_CLK_PIN	GPIO5
#define OLED_MISO_PIN	GPIO6
#define OLED_MOSI_PIN	GPIO7

#define OLED_COLS	64
#define OLED_ROWS	128
#define OLED_PAGE_START 0xB0
#define OLED_PAGE_END	0xB7

class Oled {
private:
    void inline init_gpio(void);
    void inline init_spi(void);
 public:
    void init(void);
    void wr_cmd(uint8_t data);
    void wr_data(uint8_t data);
    void clear(void);
    void update(uint8_t *internalOledArray);
};

#endif
