/* Board support for generic AFTx07 board */

#include "boardsupport.h"
#include "format.h"
#include <stdint.h>
#include <support.h>

// Generates a function that returns a u64 from a CSR
#define GET_CSR(type)                                                                              \
    static inline uint32_t get_##type() {                                                          \
        uint32_t lo;                                                                               \
        __asm__ volatile("csrr %0, " #type : "=r"(lo));                                            \
        return lo;                                                                                 \
    }

static uint32_t start_cycles;
static uint32_t start_instrs;

// Generate functions to get cycle counter and number of instructions retired
GET_CSR(cycle)
GET_CSR(instret)

void __attribute__((interrupt)) __attribute__((aligned(4))) handler() {
    uint32_t epc_value;
    uint32_t cause_value;
    asm volatile("csrr %0, mepc" : "=r"(epc_value));
    asm volatile("csrr %0, mcause" : "=r"(cause_value));
    print("mepc: %d\n", epc_value);
    print("mcause: %d\n", cause_value);
    // read misa and see if supervisor is supported
    uint32_t misa_value;
    uint32_t smode_val = 1 << 18;
    asm volatile("csrr %0, misa" : "=r"(misa_value));
    if (misa_value & smode_val) {
        asm volatile("csrr %0, sepc" : "=r"(epc_value));
        asm volatile("csrr %0, scause" : "=r"(cause_value));
        print("sepc: %d\n", epc_value);
        print("scause: %d\n", cause_value);
    }
    while(1) {}
}

void initialise_board(void) {
}

void print_verify_benchmark(int res) {
    print("Benchmark Return: %d", res);
}

void __attribute__((noinline)) __attribute__((externally_visible)) start_trigger(void) {
    print("Start trigger!\n");
    start_cycles = get_cycle();
    start_instrs = get_instret();
}

void __attribute__((noinline)) __attribute__((externally_visible)) stop_trigger(void) {
    uint32_t end_cycles, end_instrs;
    end_cycles = get_cycle();
    end_instrs = get_instret();
    print("Total Cycles: %u\n", end_cycles - start_cycles);
    print("Total Instructions: %u\n", end_instrs - start_instrs);
}
