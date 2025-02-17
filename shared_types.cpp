#import "shared_types.h"
#import "bitmap.h"

Coord Coord::operator+(const Coord &rhs) const {
  return {x + rhs.x, y + rhs.y};
}

Canvas::Canvas(int width, int height) {
  canvas = std::vector<std::vector<RgbColor>>(
      width, std::vector<RgbColor>(height, {0, 0, 0}));
  dim = {.w = width, .h = height};

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      set_pixel_safe({i, j}, {255, 255, 255});
    }
  }
}

Canvas::Canvas(const Dimensions &dim) {
  canvas = std::vector<std::vector<RgbColor>>(
      dim.w, std::vector<RgbColor>(dim.h, {0, 0, 0}));
  this->dim = dim;

  for (int i = 0; i < dim.w; i++) {
    for (int j = 0; j < dim.h; j++) {
      set_pixel_safe({i, j}, {255, 255, 255});
    }
  }
}

void Canvas::set_pixel_safe(const Coord &coord, const RgbColor &color) {
  if (coord.x < 0 || coord.y < 0)
    return;
  if (coord.x >= dim.w || coord.y >= dim.h)
    return;
  canvas[coord.x][coord.y] = color;
}

RgbColor Canvas::get(int x, int y) const { return canvas[x][y]; }

RgbColor Canvas::get(const Coord &coord) const {
  return canvas[coord.x][coord.y];
}

void Canvas::set(int x, int y, const RgbColor &color) {
  set_pixel_safe({x, y}, color);
}

void Canvas::set(const Coord &coord, const RgbColor &color) {
  set_pixel_safe(coord, color);
}
