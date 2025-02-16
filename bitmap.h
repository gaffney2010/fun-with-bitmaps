#ifndef BITMAP_H
#define BITMAP_H

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

struct RgbColor {
  unsigned char r, g, b;
};

typedef std::vector<std::vector<RgbColor>> Canvas;

std::pair<int, int> width_height(std::string filepath);
Canvas load_image(std::string filepath);

#endif // BITMAP_H

