.set MULTIBOOT_MAGIC, 0x1BADB002
.set MULTIBOOT_FLAGS, 0
.set MULTIBOOT_CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
.set STACK_SIZE, 16384 # 16 KB

/*
A multiboot header following the specs. We don't set any flags for this simple kernel.
*/
.section .multiboot
.align 4
.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long MULTIBOOT_CHECKSUM

/*
Allocate space for the stack. We set the stack pointer in the `_start` function.
*/
.section .stack,"aw",@nobits
.align 16
.skip STACK_SIZE
stack_top:

/*
The entrypoint
*/
.section .text
.global _start
.type _start, @function
_start:
  mov $stack_top, %esp  # Initialize the stack
  call kernel_main  # Enter the high-level kernel

  /* Enter an infinite halt loop */
  cli
1:  hlt
  jmp 1b
