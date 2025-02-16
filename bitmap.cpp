#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#pragma pack(push, 1) // Ensures structure alignment matches BMP file
struct BMPHeader {
  uint16_t fileType; // Should be 'BM' (0x4D42)
  uint32_t fileSize;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t offsetData; // Start of pixel data
};

struct DIBHeader {
  uint32_t size;
  int32_t width;
  int32_t height;
  uint16_t planes;
  uint16_t bitCount; // Bits per pixel (24 for RGB)
  uint32_t compression;
  uint32_t imageSize;
  int32_t xPixelsPerMeter;
  int32_t yPixelsPerMeter;
  uint32_t colorsUsed;
  uint32_t colorsImportant;
};
#pragma pack(pop)

struct BgrColor {
  unsigned char b, g, r;
};

struct RgbColor {
  unsigned char r, g, b;
};

inline RgbColor fix_color(const BgrColor &color) {
  return {color.r, color.g, color.b};
}

std::pair<BMPHeader, DIBHeader> read_headers(std::ifstream &file) {
  // Read headers
  BMPHeader bmp_header;
  DIBHeader dib_header;
  file.read(reinterpret_cast<char *>(&bmp_header), sizeof(bmp_header));
  file.read(reinterpret_cast<char *>(&dib_header), sizeof(dib_header));

  // Validate BMP format
  if (bmp_header.fileType != 0x4D42) {
    throw std::runtime_error("Error: Not a valid BMP file!");
  }
  if (dib_header.bitCount != 24) {
    throw std::runtime_error("Error: Only 24-bit BMP is supported!");
  }

  return {bmp_header, dib_header};
}

std::pair<int, int> width_height(std::string filepath) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Error: Could not open file: " + filepath);
  }

  auto [bmp_header, dib_header] = read_headers(file);

  file.close();

  return {dib_header.width, dib_header.height};
}

RgbColor **load_image(std::string filepath) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Error: Could not open file: " + filepath);
  }

  auto [bmp_header, dib_header] = read_headers(file);

  int width = dib_header.width;
  int height = dib_header.height;
  RgbColor **result = new RgbColor *[width];
  for (int i = 0; i < width; i++) {
    result[i] = new RgbColor[height];
  }

  file.seekg(bmp_header.offsetData, std::ios::beg);
  int rowPadding =
      (4 - (width * 3) % 4) % 4; // BMP rows are padded to 4-byte alignment

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      BgrColor pixel;
      file.read(reinterpret_cast<char *>(&pixel), sizeof(BgrColor));
      result[x][y] = fix_color(pixel);
    }
    file.ignore(rowPadding); // Skip padding bytes
  }

  file.close();

  return result;
}

