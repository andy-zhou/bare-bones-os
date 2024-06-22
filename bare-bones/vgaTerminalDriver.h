#pragma once

#include <cstdint>
#include <cstddef>

enum vgaColor
{
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15
};

class VGATerminalDriver
{
private:
  struct Coordinate
  {
    size_t row;
    size_t column;
  };

  const size_t rows;
  const size_t columns;
  uint16_t *const textBuffer;
  uint8_t color;
  Coordinate cursor;

  void putEntryAt(char c, uint8_t color, Coordinate coordinate);
  void putChar(char c);
  inline void advanceCursor();
  inline void advanceCursorLine();

public:
  VGATerminalDriver(size_t rows, size_t columns);
  void writeString(const char *data);
  void setColor(uint8_t color);
};