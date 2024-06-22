#include "vgaTerminalDriver.h"
#include "strlen.h"

namespace
{
  inline uint8_t vgaEntryColor(enum vgaColor fg, enum vgaColor bg)
  {
    return fg | bg << 4;
  }

  inline uint16_t vgaEntry(unsigned char character, uint8_t color)
  {
    return static_cast<uint16_t>(character) | static_cast<uint16_t>(color) << 8;
  }
}

VGATerminalDriver::VGATerminalDriver(size_t rows, size_t columns)
    : rows(rows),
      columns(columns),
      textBuffer(reinterpret_cast<uint16_t *>(0xB8000)),
      color(vgaEntryColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK)),
      cursor({0, 0})
{
  for (size_t row = 0; row < this->rows; row++)
  {
    for (size_t column = 0; column < this->columns; column++)
    {
      this->putEntryAt(' ', this->color, {row, column});
    }
  }
}

void VGATerminalDriver::putEntryAt(char character, uint8_t color, Coordinate coordinate)
{
  const auto index = coordinate.row + coordinate.column * this->rows;
  const auto entry = vgaEntry(character, color);
  this->textBuffer[index] = entry;
};

void VGATerminalDriver::advanceCursor()
{
  if (this->cursor.column + 1 >= this->columns)
  {
    this->advanceCursorLine();
  }
  else
  {
    ++this->cursor.column;
  }
};

void VGATerminalDriver::advanceCursorLine()
{
  this->cursor.column = 0;
  ++this->cursor.row;
}

void VGATerminalDriver::putChar(char character)
{
  this->putEntryAt(character, this->color, this->cursor);
}

void VGATerminalDriver::writeString(const char *data)
{
  const auto length = strlen(data);
  for (size_t i = 0; i < length; ++i)
  {
    this->putChar(data[i]);
    if (data[i] == '\n')
    {
      this->advanceCursorLine();
    }
    else
    {
      this->advanceCursor();
    }
  }
}

void VGATerminalDriver::setColor(uint8_t color)
{
  this->color = color;
}