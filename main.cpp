#include "raylib.h"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <optional>
#include <vector>

#include "shared_types.h"
#include "bitmap.h"

struct WindowParams {
  Dimensions dim;
  Canvas target_canvas;
};

struct Candidate {
  Coord coord;
  RgbColor color;
  int radius;
};

std::vector<Coord> circle_mask(const Coord &coord, int radius,
                               const Dimensions& dim) {
  std::vector<Coord> result;
  for (int dy = -radius; dy <= radius; dy++) {
    for (int dx = -radius; dx <= radius; dx++) {
      if (dx * dx + dy * dy > radius * radius)
        continue;
      Coord d = {dx, dy};
      Coord r = coord + d;
      if (r.x < 0 || r.y < 0)
        continue;
      if (r.x >= dim.w || r.y >= dim.h)
        continue;
      result.push_back(r);
    }
  }
  return result;
}

std::vector<Coord> circle_mask(const Candidate &candidate,
                               const Dimensions& dim) {
  return circle_mask(candidate.coord, candidate.radius, dim);
}

int diff(const RgbColor &apx, const RgbColor &bpx) {
  int max = 0;
  if (max < abs(apx.r - bpx.r))
    max = abs(apx.r - bpx.r);
  if (max < abs(apx.g - bpx.g))
    max = abs(apx.g - bpx.g);
  if (max < abs(apx.b - bpx.b))
    max = abs(apx.b - bpx.b);
  return max;
}

int diff(const Canvas &a, const Canvas &b, const std::vector<Coord> &mask) {
  int max = 0;
  for (const auto &coord : mask) {
    int this_diff = diff(a.get(coord), b.get(coord));
    if (max < this_diff)
      max = this_diff;
  }
  return max;
}

std::vector<Candidate> generate_candidates(int step, const WindowParams &wp) {
  return {{
      .coord = {.x = rand() % wp.dim.w, .y = rand() % wp.dim.h},
      .color =
          {
              .r = static_cast<unsigned char>(rand() % 256),
              .g = static_cast<unsigned char>(rand() % 256),
              .b = static_cast<unsigned char>(rand() % 256),
          },
      .radius = 10,
  }};
}

Canvas canvas_from_color_mask(const RgbColor &color,
                              const std::vector<Coord> &mask,
                              const WindowParams &wp) {
  Canvas result(wp.dim);
  for (const auto &coord : mask) {
    result.set(coord, color);
  }
  return result;
}

std::optional<Candidate>
best_candidate(const std::vector<Candidate> &candidates, const Canvas &canvas,
               const WindowParams &wp) {
  std::optional<Candidate> result = std::nullopt;
  for (const auto &candidate : candidates) {
    auto this_mask = circle_mask(candidate, wp.dim);
    auto new_canvas = canvas_from_color_mask(candidate.color, this_mask, wp);
    if (diff(new_canvas, wp.target_canvas, this_mask) <
        diff(canvas, wp.target_canvas, this_mask))
      result = candidate;
  }
  return result;
}

Candidate find_candidate(int step, const Canvas &canvas,
                         const WindowParams &wp) {
  std::optional<Candidate> result = std::nullopt;
  while (!result.has_value()) {
    result = best_candidate(generate_candidates(step, wp), canvas, wp);
    // std::cout << "Candidate did not work out." << std::endl;
  }
  return *result;
}

inline Color color_from_rgb_color(const RgbColor &rgb_color) {
  return {rgb_color.r, rgb_color.g, rgb_color.b, 255};
}

int main() {
  // Initialize random seed
  std::srand(std::time(nullptr));

  // Set window dimensions
  const int w = 800;
  const int h = 600;
  WindowParams wp = {
      .dim = {.w = w, .h = h}, .target_canvas = load_image("starry_night.bmp")};
  InitWindow(wp.dim.w, wp.dim.h, "Fun with bitmaps");

  Canvas canvas(wp.dim);

  SetTargetFPS(1);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    // Add a new circle to the canvas
    auto c = find_candidate(0, canvas, wp);
    for (const auto &coord : circle_mask(c.coord, c.radius, wp.dim)) {
      canvas.set(coord, c.color);
    }

    // Redraw everything
    for (int i = 0; i < wp.dim.w; i++) {
      for (int j = 0; j < wp.dim.h; j++) {
        DrawPixel(i, j, color_from_rgb_color(canvas.get(i, j)));
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
