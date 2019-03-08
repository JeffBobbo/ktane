#ifndef PATHS_H_INCLUDE
#define PATHS_H_INCLUDE

#include <stdint.h>

using Cell = uint8_t;

constexpr Cell _cell(uint8_t y, uint8_t x) {
  return y * 8 + (7 - x);
}

Cell path[36] = {
  _cell(0, 4),
  _cell(1, 4), _cell(1, 5),
  _cell(2, 5), _cell(2, 6),
  _cell(3, 6),
  _cell(4, 6), _cell(4, 7),
  _cell(5, 7),
  _cell(6, 7), _cell(6, 6), _cell(6, 5), _cell(6, 4),
  _cell(5, 4),
  _cell(4, 4), _cell(4, 3),
  _cell(3, 3),
  _cell(2, 3), _cell(2, 2), _cell(2, 1),
  _cell(3, 1), _cell(3, 0),
  _cell(4, 0),
  _cell(5, 0), _cell(5, 1),
  _cell(6, 1), _cell(6, 2),
  _cell(7, 2),
  255
};

#endif
