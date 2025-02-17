#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include <vector>

struct RgbColor {
  unsigned char r, g, b;
};

struct Dimensions {
  int w, h;
};

struct Coord {
  int x, y;

  Coord operator+(const Coord &rhs) const;
};

class Canvas {
private:
  Dimensions dim;
  std::vector<std::vector<RgbColor>> canvas;
  void set_pixel_safe(const Coord &coord, const RgbColor &color);

public:
  // Constructor
  Canvas(int width, int height);
  Canvas(const Dimensions &dim);

  RgbColor get(int x, int y) const;
  RgbColor get(const Coord &coord) const;
  void set(int x, int y, const RgbColor &color);
  void set(const Coord &coord, const RgbColor &color);
};

#endif // SHARED_TYPES_H
