#include <cstddef>
#include <cstdint>
#include "vgaTerminalDriver.h"

/* Compiler warnings based on pre-defined compiler macros */
/* https://sourceforge.net/p/predef/wiki/OperatingSystems/ */
#if !defined(__INTELLISENSE__) && defined(__APPLE__)
#error "Not using cross-compiler"
#endif
#if !defined(__INTELLISENSE__) && !defined(__i386__)
#error "Must be compiled for an i386 target"
#endif

namespace
{
  constexpr size_t SCREEN_WIDTH = 80;
  constexpr size_t SCREEN_HEIGHT = 25;
}

extern "C" void kernel_main()
{
  VGATerminalDriver terminalDriver(SCREEN_WIDTH, SCREEN_HEIGHT);
  terminalDriver.writeString("Hello\nWorld!");
}