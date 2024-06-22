# Bare Bones

A simple kernel that does nothing but print a message to the screen. Follows [this guide](https://wiki.osdev.org/Bare_Bones)

## `boot.s`

A simple file in GAS syntax that does the following:

1. Declares a multiboot header.
2. Sets up a stack for the kernel.
3. Defines the `_start` symbol, which calls the `kernel_main` function.

### Multiboot Header

Per the [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html):

> An OS image may be an ordinary 32-bit executable file in the standard format for that particular operating system, except that it may be linked at a non-default load address to avoid loading on top of the PC’s I/O region or other reserved areas, and of course it should not use shared libraries or other fancy features.
>
> An OS image must contain an additional header called Multiboot header, besides the headers of the format used by the OS image. The Multiboot header must be contained completely within the first 8192 bytes of the OS image, and must be longword (32-bit) aligned. In general, it should come as early as possible, and may be embedded in the beginning of the text segment after the real executable header.

#### Required Fields

Specifically, the multiboot header must define the following [required fields](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Header-magic-fields):

| Offset | Type  | Field      | Description                            |
| ------ | ----- | ---------- | -------------------------------------- |
| 0      | `u32` | `magic`    | Must be `0x1BADB002`.                  |
| 4      | `u32` | `flags`    | Flags for the multiboot header.        |
| 8      | `u32` | `checksum` | The 32-bit sum of `magic` and `flags`. |

#### Header Flags

The header flags specify the features an OS image wants from the bootloader. In general, bits 0-15 indicate required features and bits 16-31 indicate optional features.

The following flags are special:

| Bit | Constant Name           | Description                                                                                                                                                                                                                                                                                       |
| --- | ----------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0   | `MULTIBOOT_PAGE_ALIGN`  | Align the OS image on page (4KB) boundaries.                                                                                                                                                                                                                                                      |
| 1   | `MULTIBOOT_MEMORY_INFO` | Include memory information. See [Boot Information Format](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format), specifically the `mem_*` and `mmap_*` fields.                                                                                               |
| 2   | `MULTIBOOT_VIDEO_MODE`  | Include video mode information. See [Boot Information Format](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format), specifically the `vbe_*` fields.                                                                                                        |
| 16  | `MULTIBOOT_AOUT_KLUDGE` | The OS image is in a format that is not the standard ELF format. This flag is only valid if the OS image is not in the standard ELF format. See [Header Address Fields](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Header-address-fields) for fields enabled by this flag. |

### Stack Setup

The multiboot specification does not set up a stack for the kernel, so we must do it ourselves. A few notes on the stack:

- The stack on x86 grows downward, so we initialize the stack pointer (`esp`) to the top of the stack.
- The stack must be 16-byte aligned for modern versions of GCC targeting i386. Previous versions of GCC required only 4-byte alignment. See [System V ABI](https://wiki.osdev.org/System_V_ABI#i386).

In the wiki tutorial, the stack is set up in the `.bss` section, but we use a custom `.stack` section here for clarity, particularly in the
linker script. The `.bss` section is marked as `NOBITS` by default, which means it does not occupy space in the binary file. In order to emulate
this behavior, we use the `@nobits` type argument in the `.section` directive.

For more information on the `.section` directive, refer to the [GAS documentation](https://sourceware.org/binutils/docs/as/Section.html#ELF-Version).

### Entry Point

Here we define our entry point, `_start`. In user-space programs, the `_start` symbol is provided by the C runtime, but in a kernel, we must define it ourselves.

In start, we call the `kernel_main` function, which is defined in `kernel.c`. This function is the main entry point for the
kernel. As high-level code, we must ensure that the stack is 16 byte aligned before calling this function.

Note that this function does not return, since there is no operating system to return to. Instead, we enter an infinite halt loop, which is a common pattern in bare-metal programming. The basic steps are:

1. Disable maskable interrupts (also already disabled by the bootloader).
2. Halt the CPU until the next interrupt. Because maskable interrupts are disabled, this will halt the CPU indefinitely or until a non-maskable interrupt occurs.
3. If a non-maskable interrupt occurs, jump back to the halt instruction.

## `vgaTerminalDriver.cc`

A simple driver that prints strings to the screen. This is a basic VGA text mode driver that writes to the text buffer directly. See [VGA Text Mode](https://en.wikipedia.org/wiki/VGA_text_mode) for more information.

## `kernel.cc`

The main kernel file that defines the `kernel_main` function. This function initializes the VGA terminal driver and prints a message to the screen.

## `linker.ld`

A linker script that defines the layout of the kernel binary.

## `grub.cfg`

See [GRUB Configuration](https://www.gnu.org/software/grub/manual/grub/grub.html#Configuration).

## Building the Kernel

Here are the steps to build the kernel:

### Assembling

```console
i686-elf-as bare-bones/boot.s -o bare-bones/build/boot.o
```

### Compiling

#### VGA Terminal Driver

```console
i686-elf-g++ -c bare-bones/vgaTerminalDriver.cc -o bare-bones/build/vgaTerminalDriver.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
```

#### Kernel

```console
i686-elf-g++ -c bare-bones/vgaTerminalDriver.cc -o bare-bones/build/vgaTerminalDriver.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
```

### Linking

```console
i686-elf-gcc -T bare-bones/linker.ld -o bare-bones/build/boneos.bin -ffreestanding -O2 -nostdlib bare-bones/build/boot.o bare-bones/build/kernel.o bare-bones/build/vgaTerminalDriver.o -lgcc
```

### Examining the Binary

```console
objdump -D boneos.bin --demangle
```

### Check for Multiboot Header

```console
artifacts/bin/grub-file --is-x86-multiboot bare-bones/build/boneos.bin
```

### Building the ISO

```console
mkdir -p bare-bones/build/image/boot/grub
cp bare-bones/build/boneos.bin bare-bones/build/image/boot/boneos.bin
cp bare-bones/grub.cfg bare-bones/build/image/boot/grub/grub.cfg
./artifacts/bin/grub-mkrescue -o bare-bones/build/boneos.iso bare-bones/build/image
```
