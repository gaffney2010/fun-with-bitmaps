#ifndef BITMAP_H
#define BITMAP_H

#include "shared_types.h"

std::pair<int, int> width_height(std::string filepath);
Canvas load_image(std::string filepath);

#endif // BITMAP_H

