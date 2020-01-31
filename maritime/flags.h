#ifndef FLAGS_H_INCLUDE
#define FLAGS_H_INCLUDE

#include <stdint.h>

class Adafruit_ST7735;
void flag(Adafruit_ST7735& tft, const char f);

constexpr uint16_t colour(const float r, const float g, const float b);
constexpr uint16_t COLOUR_BLACK   = 0x0000;
constexpr uint16_t COLOUR_BLUE    = 0x001F;
constexpr uint16_t COLOUR_RED     = 0xF800;
constexpr uint16_t COLOUR_GREEN   = 0x07E0;
constexpr uint16_t COLOUR_CYAN    = 0x07FF;
constexpr uint16_t COLOUR_MAGENTA = 0xF81F;
constexpr uint16_t COLOUR_YELLOW  = 0xFFE0;
constexpr uint16_t COLOUR_WHITE   = 0xFFFF;

#endif
