#define CPU_MHZ 1

#define RVB_RETURN (return_val)                              \
    asm volatile ("mv a0, %0" : : "r" (return_val ? 1 : 0)); \
    asm volatile ("ecall");                                  \
