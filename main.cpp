#include "raylib.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <optional>
#include <vector>

#include "bitmap.h"
#include "shared_types.h"

struct WindowParams {
  Dimensions dim;
  Canvas target_canvas;
};

struct Candidate {
  Coord coord;
  RgbColor color;
};

std::vector<Coord> circle_mask(const Coord &coord, int radius,
                               const Dimensions &dim) {
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
    if (coord.x % 2 == 0 && coord.y % 2 == 0) {
      int this_diff = diff(a.get(coord), b.get(coord));
      if (max < this_diff)
        max = this_diff;
    }
  }
  return max;
}

std::vector<Candidate> generate_candidates(int step, const WindowParams &wp) {
  std::vector<Candidate> result;
  for (int i = 0; i < 3; i++) {
    Coord coord = {rand() % wp.dim.w, rand() % wp.dim.h};
    result.push_back({
        .coord = coord,
        .color = wp.target_canvas.get(coord),
    });
  }
  return result;
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
               int radius, const WindowParams &wp) {
  if (radius <= 8) return candidates[0];
  std::optional<Candidate> result = std::nullopt;
  int record = 0;
  for (const auto &candidate : candidates) {
    auto this_mask = circle_mask(candidate.coord, radius, wp.dim);
    auto new_canvas = canvas_from_color_mask(candidate.color, this_mask, wp);
    int improvement = diff(canvas, wp.target_canvas, this_mask) -
                      diff(new_canvas, wp.target_canvas, this_mask);
    if (improvement > record) {
      record = improvement;
      result = candidate;
    }
  }
  return result;
}

Candidate find_candidate(int step, const Canvas &canvas, const WindowParams &wp,
                         int &radius) {
  std::optional<Candidate> result = std::nullopt;
  // if (step >= 1000) radius = 2;
  int failure = 0;
  while (!result.has_value()) {
    result = best_candidate(generate_candidates(step, wp), canvas, radius, wp);
    failure++;
    if (failure > 10 && radius > 3) {
      radius--;
      failure = 0;
    }
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
  WindowParams wp = {.dim = {.w = w, .h = h},
                     .target_canvas = load_image("starry_night.bmp")};
  InitWindow(wp.dim.w, wp.dim.h, "Fun with bitmaps");

  Canvas canvas(wp.dim);

  SetTargetFPS(120);

  int radius = 100;

  int step = 0;
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    // Add a new circle to the canvas
    auto c = find_candidate(step, canvas, wp, radius);
    step++;
    for (const auto &coord : circle_mask(c.coord, radius, wp.dim)) {
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
