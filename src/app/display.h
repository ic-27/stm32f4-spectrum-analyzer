#ifndef DISPLAY_H
#define DISPLAY_H

#include "../driver/oled.h"

class Display : private Oled {
 public:
    Display(void);
    void init(void);
    void update(uint8_t *internalOledArray);
};

#endif
