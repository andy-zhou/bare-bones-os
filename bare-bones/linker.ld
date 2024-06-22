/* Set the _start symbol defined in boot.s as the entrypoint. */
ENTRY(_start)

SECTIONS {
  /* Offset by 2M */
  . = 2M;

  /* Begin with the multiboot header */
  .text : {
    *(.multiboot)
    *(.text)
  }

  .rodata : {
    *(.rodata)
  }

  .data : {
    *(.data)
  }

  /* Add the stack */
  .bss : {
    *(COMMON)
    *(.bss)
    *(.stack)
  }
}