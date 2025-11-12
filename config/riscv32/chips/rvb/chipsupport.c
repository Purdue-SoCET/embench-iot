#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "chipsupport.h"

void trap_entry();
void pop_tf(trapframe_t*);

extern volatile uint64_t tohost;
extern volatile uint64_t fromhost;

static void do_tohost(uint64_t tohost_value)
{
  tohost = tohost_value;
}

static uint64_t lfsr63(uint64_t x)
{
  uint64_t bit = (x ^ (x >> 1)) & 1;
  return (x >> 1) | (bit << 62);
}

static void cputchar(int x)
{
  do_tohost(0x0101000000000000 | (unsigned char)x);
  asm volatile ("fence.i" : : :);
}

static void cputstring(const char* s)
{
  size_t len = strlen(s);
  for (size_t i = 0; i < len; i++) {
    cputchar(*s++);  
  }
  cputchar('\n');
}

static void terminate(uint32_t x)
{
  do_tohost(x);
  asm volatile ("fence.i" : : :);
  while (1);
}

void wtf()
{
  terminate(841);
}

#define stringify1(x) #x
#define stringify(x) stringify1(x)
#define assert(x) do { \
  if (x) break; \
  cputstring("Assertion failed: " stringify(x) "\n"); \
  terminate(3); \
} while(0)

void printhex(uint64_t x)
{
  char str[17];
  for (int i = 0; i < 16; i++)
  {
    str[15-i] = (x & 0xF) + ((x & 0xF) < 10 ? '0' : 'a'-10);
    x >>= 4;
  }
  str[16] = 0;

  cputstring(str);
}

extern int pf_filter(uintptr_t addr, uintptr_t *pte, int *copy);
extern int trap_filter(trapframe_t *tf);

void handle_trap(trapframe_t* tf)
{
  if (trap_filter(tf)) {
    pop_tf(tf);
  }

  if (tf->cause == CAUSE_USER_ECALL)
  {
    int correct = tf->gpr[10];

    terminate(correct);
  }
  else if (tf->cause == CAUSE_ILLEGAL_INSTRUCTION)
  {
    assert(!"illegal instruction");
    tf->epc += 4;
  }
  else if (tf->cause == CAUSE_FETCH_PAGE_FAULT || tf->cause == CAUSE_LOAD_PAGE_FAULT || tf->cause == CAUSE_STORE_PAGE_FAULT)
    assert(!"unexpected page fault");
  else
    assert(!"unexpected exception");

  pop_tf(tf);
}

void boot(uintptr_t test_addr)
{
  _Static_assert(SIZEOF_TRAPFRAME_T == sizeof(trapframe_t), "???");

  // Set up PMPs if present, ignoring illegal instruction trap if not.
  uintptr_t pmpc = PMP_NAPOT | PMP_R | PMP_W | PMP_X;
  uintptr_t pmpa = ((uintptr_t)1 << (__riscv_xlen == 32 ? 31 : 53)) - 1;
  asm volatile ("la t0, 1f\n\t"
                "csrrw t0, mtvec, t0\n\t"
                "csrw pmpaddr0, %1\n\t"
                "csrw pmpcfg0, %0\n\t"
                ".align 2\n\t"
                "1: csrw mtvec, t0"
                : : "r" (pmpc), "r" (pmpa) : "t0");

  // set up trap handling
  write_csr(mtvec, trap_entry);
  write_csr(mscratch, read_csr(mscratch));
  // FPU on; accelerator on; vector unit on
  write_csr(mstatus, MSTATUS_FS | MSTATUS_XS | MSTATUS_VS);
  write_csr(mie, 0);

  trapframe_t tf;
  memset(&tf, 0, sizeof(tf));
  tf.epc = test_addr;
  tf.gpr[2] = 0x80020000;
  pop_tf(&tf);
}