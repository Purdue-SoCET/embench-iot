OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
  . = 0x80000000;
  .text.init : { *(.text.init) }
  . = ALIGN(0x1000);
  .tohost : { *(.tohost) }
  . = ALIGN(0x1000);
  .text : { *(.text) }
  . = ALIGN(0x1000);
  .preinit_array :
  {
      PROVIDE(__preinit_array_start = .);
      KEEP(*(SORT_BY_INIT_PRIORITY(.preinit_array)));
      PROVIDE(__preinit_array_end = .);
  }
  .init_array :
  {
      PROVIDE(__init_array_start = .);
      KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
      KEEP(*(.init_array .ctors));
      PROVIDE(__init_array_end = .);
  }
  .fini_array :
  {
      PROVIDE(__fini_array_start = .);
      KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)));
      KEEP(*(.fini_array .dtors));
      PROVIDE(__fini_array_end = .);
  }
  . = ALIGN(0x1000);
  .data : { *(.data .data.* .rodata .rodata.* .eh_frame .eh_frame.*); }
  PROVIDE(_edata = .);
  PROVIDE(__bss_start = .);
  .bss : { *(.sbss .sbss.*) *(.bss) }
  PROVIDE(__bss_end = .);
  PROVIDE(_end = .);
  PROVIDE(__global_pointer$ = .);
  . = ALIGN(0x1000);
  PROVIDE(__heap_start = .);
}