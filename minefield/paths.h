#ifndef PATHS_H_INCLUDE
#define PATHS_H_INCLUDE

#include <stdint.h>

using Cell = uint8_t;

struct Path
{
  Path() : length(0), path(nullptr) {}
  Path(const uint8_t l, const Cell* p) : length(l), path(p) {}

  uint8_t length;
  Cell* path;
};

const uint8_t NUM_PATHS = 8;
extern Path paths[NUM_PATHS];

#endif
