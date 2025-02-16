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

struct Candidate {
  int x, y;
  RgbColor color;
};

std::vector<Candidate> generate_candidates(int step, const WindowParams &wp) {
  return {{.x = rand() % wp.w,
           .y = rand() % wp.h,
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

inline Color color_from_rgb_color(const RgbColor &rgb_color) {
  return {rgb_color.r, rgb_color.g, rgb_color.b, 255};
}

void set_canvas_pixel(RgbColor **canvas, RgbColor color, int i, int j,
                      const WindowParams &wp) {
  if (i < 0 || j < 0)
    return;
  if (i >= wp.w || j >= wp.h)
    return;
  canvas[i][j] = color;
}

int main() {
  // Initialize random seed
  std::srand(std::time(nullptr));

  // Set window dimensions
  const int w = 800;
  const int h = 600;
  WindowParams wp = {.w = w, .h = h};
  InitWindow(wp.w, wp.h, "Fun with bitmaps");
  // TJ: Replace widht height with w h and use wp below
  // TJ: Fix width / height

  // Allocate a 2D array of Color
  RgbColor **canvas = new RgbColor *[wp.w];
  for (int i = 0; i < wp.w; i++) {
    canvas[i] = new RgbColor[wp.h];
  }

  // Set all pixels initally to black
  for (int i = 0; i < wp.w; i++) {
    for (int j = 0; j < wp.h; j++) {
      set_canvas_pixel(canvas, {255, 255, 255}, i, j, wp);
    }
  }

  int radius = 100;

  SetTargetFPS(1);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw the circle pixel-by-pixel using the Midpoint Circle Algorithm
    auto c = best_candidate(generate_candidates(0, wp));
    assert(c.has_value());
    for (int dy = -radius; dy <= radius; dy++) {
      for (int dx = -radius; dx <= radius; dx++) {
        if (dx * dx + dy * dy <= radius * radius) {
          set_canvas_pixel(canvas, c->color, c->x + dx, c->y + dy, wp);
        }
      }
    }

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
