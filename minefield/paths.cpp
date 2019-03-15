#include "paths.h"

constexpr Cell _cell(uint8_t x, uint8_t y) {
  return y * 8 + (7 - x);
}

Cell path0[] = {
  _cell(0, 0),
  _cell(1, 0),
  _cell(1, 1),
  _cell(1, 2),
  _cell(2, 2),
  _cell(3, 2),
  _cell(4, 2),
  _cell(4, 3),
  _cell(4, 4),
  _cell(3, 4),
  _cell(2, 4),
  _cell(2, 5),
  _cell(2, 6),
  _cell(3, 6),
  _cell(4, 6),
  _cell(5, 6),
  _cell(5, 7)
};
Cell path1[] = {
  _cell(1, 0),
  _cell(2, 0),
  _cell(3, 0),
  _cell(4, 0),
  _cell(4, 1),
  _cell(5, 1),
  _cell(5, 2),
  _cell(6, 2),
  _cell(7, 2),
  _cell(7, 3),
  _cell(7, 4),
  _cell(7, 5),
  _cell(7, 6),
  _cell(6, 6),
  _cell(6, 7)
};
Cell path2[] = {
  _cell(2, 0),
  _cell(2, 1),
  _cell(2, 2),
  _cell(1, 2),
  _cell(1, 3),
  _cell(1, 4),
  _cell(2, 4),
  _cell(3, 4),
  _cell(4, 4),
  _cell(4, 3),
  _cell(4, 2),
  _cell(5, 2),
  _cell(6, 2),
  _cell(6, 3),
  _cell(6, 4),
  _cell(6, 5),
  _cell(6, 6),
  _cell(6, 7)
};
Cell path3[] = {
  _cell(3, 0),
  _cell(2, 0),
  _cell(2, 1),
  _cell(2, 2),
  _cell(2, 3),
  _cell(3, 3),
  _cell(4, 3),
  _cell(4, 4),
  _cell(5, 4),
  _cell(5, 5),
  _cell(5, 6),
  _cell(4, 6),
  _cell(4, 7)
};
Cell path4[] = {
  _cell(4, 0),
  _cell(4, 1),
  _cell(5, 1),
  _cell(5, 2),
  _cell(6, 2),
  _cell(6, 3),
  _cell(6, 4),
  _cell(7, 4),
  _cell(7, 5),
  _cell(7, 6),
  _cell(6, 6),
  _cell(5, 6),
  _cell(4, 6),
  _cell(4, 5),
  _cell(4, 4),
  _cell(3, 4),
  _cell(3, 3),
  _cell(3, 2),
  _cell(2, 2),
  _cell(1, 2),
  _cell(1, 3),
  _cell(0, 3),
  _cell(0, 4),
  _cell(0, 5),
  _cell(1, 5),
  _cell(1, 6),
  _cell(2, 6),
  _cell(2, 7)
};
Cell path5[] = {
  _cell(5, 0),
  _cell(5, 1),
  _cell(4, 1),
  _cell(3, 1),
  _cell(3, 0),
  _cell(2, 0),
  _cell(1, 0),
  _cell(1, 1),
  _cell(0, 1),
  _cell(0, 2),
  _cell(0, 3),
  _cell(0, 4),
  _cell(1, 4),
  _cell(1, 5),
  _cell(2, 5),
  _cell(3, 5),
  _cell(3, 6),
  _cell(3, 7)
};
Cell path6 = {
  _cell(6, 0),
  _cell(6, 1),
  _cell(6, 2),
  _cell(6, 3),
  _cell(6, 4),
  _cell(6, 5),
  _cell(6, 6),
  _cell(5, 6),
  _cell(4, 6),
  _cell(4, 5),
  _cell(4, 4),
  _cell(3, 4),
  _cell(2, 4),
  _cell(2, 5),
  _cell(2, 6),
  _cell(1, 6),
  _cell(1, 7)
};
Cell path7 = {
  _cell(7, 0),
  _cell(6, 0),
  _cell(6, 1),
  _cell(5, 1),
  _cell(5, 2),
  _cell(5, 3),
  _cell(4, 3),
  _cell(3, 3),
  _cell(3, 2),
  _cell(2, 2),
  _cell(1, 2),
  _cell(1, 3),
  _cell(1, 4),
  _cell(2, 4),
  _cell(2, 5),
  _cell(3, 5),
  _cell(3, 6),
  _cell(4, 6),
  _cell(4, 7)
};


Path paths[] = {
  Path(sizeof(path0), path0),
  Path(sizeof(path1), path1),
  Path(sizeof(path2), path2),
  Path(sizeof(path3), path3),
  Path(sizeof(path4), path4),
  Path(sizeof(path5), path5),
  Path(sizeof(path6), path6),
  Path(sizeof(path7), path7)
};
