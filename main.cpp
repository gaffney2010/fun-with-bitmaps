#include "raylib.h"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <vector>

struct WindowParams {
  int w, h;
};

struct Candidate {
  int x, y;
  int r, g, b;
};

std::vector<Candidate> generate_candidates(int step, const WindowParams &wp) {
  return {{.x = rand() % wp.w,
           .y = rand() % wp.h,
           .r = rand() % 256,
           .g = rand() % 256,
           .b = rand() % 256}};
}

std::optional<Candidate>
best_candidate(const std::vector<Candidate> &candidates) {
  return candidates[0];
}

inline Color color_from_rgb(int r, int g, int b) {
  return {static_cast<unsigned char>(r), static_cast<unsigned char>(g),
          static_cast<unsigned char>(b), 255};
}

inline Color color_from_candidate(const Candidate &candidate) {
  return color_from_rgb(candidate.r, candidate.g, candidate.b);
}

void set_canvas_pixel(Color **canvas, Color color, int i, int j,
                      const WindowParams &wp) {
  if (i < 0 || j < 0)
    return;
  if (i >= wp.w || j >= wp.h)
    return;
  canvas[i][j] = color;
}

void set_canvas_pixel(Color **canvas, int r, int g, int b, int i, int j,
                      const WindowParams &wp) {
  set_canvas_pixel(canvas, color_from_rgb(r, g, b), i, j, wp);
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
  Color **canvas = new Color *[wp.w];
  for (int i = 0; i < wp.w; i++) {
    canvas[i] = new Color[wp.h];
  }

  // Set all pixels initally to black
  for (int i = 0; i < wp.w; i++) {
    for (int j = 0; j < wp.h; j++) {
      set_canvas_pixel(canvas, 255, 255, 255, i, j, wp);
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
          set_canvas_pixel(canvas, color_from_candidate(*c), c->x + dx,
                           c->y + dy, wp);
        }
      }
    }

    for (int i = 0; i < wp.w; i++) {
      for (int j = 0; j < wp.h; j++) {
        DrawPixel(i, j, canvas[i][j]);
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
