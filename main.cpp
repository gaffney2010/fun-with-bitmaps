#include "raylib.h"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <vector>

#include "bitmap.cpp"

struct WindowParams {
  int w, h;
};

struct Coord {
  int x, y;

  Coord operator+(const Coord &rhs) const { return {x + rhs.x, y + rhs.y}; }
};

struct Candidate {
  Coord coord;
  RgbColor color;
};

std::vector<Candidate> generate_candidates(int step, const WindowParams &wp) {
  return {{.coord = {.x = rand() % wp.w, .y = rand() % wp.h},
           .color = {
               .r = static_cast<unsigned char>(rand() % 256),
               .g = static_cast<unsigned char>(rand() % 256),
               .b = static_cast<unsigned char>(rand() % 256),
           }}};
}

std::optional<Candidate>
best_candidate(const std::vector<Candidate> &candidates) {
  return candidates[0];
}

Candidate find_candidate(int step, const WindowParams &wp) {
  std::optional<Candidate> result = std::nullopt;
  while (!result.has_value()) {
    result = best_candidate(generate_candidates(step, wp));
  }
  return *result;
}

inline Color color_from_rgb_color(const RgbColor &rgb_color) {
  return {rgb_color.r, rgb_color.g, rgb_color.b, 255};
}

void set_canvas_pixel(RgbColor **canvas, const RgbColor &color,
                      const Coord &coord, const WindowParams &wp) {
  if (coord.x < 0 || coord.y < 0)
    return;
  if (coord.x >= wp.w || coord.y >= wp.h)
    return;
  canvas[coord.x][coord.y] = color;
}

std::vector<Coord> circle_mask(const Coord &coord, int radius,
                               const WindowParams &wp) {
  std::vector<Coord> result;
  for (int dy = -radius; dy <= radius; dy++) {
    for (int dx = -radius; dx <= radius; dx++) {
      if (dx * dx + dy * dy > radius * radius)
        continue;
      Coord d = {dx, dy};
      Coord r = coord + d;
      if (r.x < 0 || r.y < 0)
        continue;
      if (r.x >= wp.w || r.y >= wp.h)
        continue;
      result.push_back(r);
    }
  }
  return result;
}

int main() {
  // Initialize random seed
  std::srand(std::time(nullptr));

  // Set window dimensions
  const int w = 800;
  const int h = 600;
  WindowParams wp = {.w = w, .h = h};
  InitWindow(wp.w, wp.h, "Fun with bitmaps");

  // Allocate a 2D array of Color
  RgbColor **canvas = new RgbColor *[wp.w];
  for (int i = 0; i < wp.w; i++) {
    canvas[i] = new RgbColor[wp.h];
  }

  // Set all pixels initally to white
  for (int i = 0; i < wp.w; i++) {
    for (int j = 0; j < wp.h; j++) {
      set_canvas_pixel(canvas, {255, 255, 255}, {i, j}, wp);
    }
  }

  int radius = 100;

  SetTargetFPS(1);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    // Add a new circle to the canvas
    auto c = find_candidate(0, wp);
    for (const auto &coord : circle_mask(c.coord, radius, wp)) {
      set_canvas_pixel(canvas, c.color, coord, wp);
    }

    // Redraw everything
    for (int i = 0; i < wp.w; i++) {
      for (int j = 0; j < wp.h; j++) {
        DrawPixel(i, j, color_from_rgb_color(canvas[i][j]));
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
