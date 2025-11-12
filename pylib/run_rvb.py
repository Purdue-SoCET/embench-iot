#!/usr/bin/env python3

# Python module to run programs on RISCVBusiness

# Contributor: William Cunningham <wrcunnin@purdue.edu>
#
# This file is not part of Embench.

"""
Embench module to run benchmark programs.

This version is suitable for the RISC-V Business simulator.
"""

__all__ = [
    'get_target_args',
    'build_benchmark_cmd',
    'decode_results',
]

import argparse
import copy
import re
import subprocess
import os

from embench_core import log

root = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")

rvb_core = os.getenv('RISCV_CORE')
cpu_mhz = 1
meminit_elf = ['riscv64-unknown-elf-objcopy', '-O', 'binary']
dump_bin = ['riscv64-unknown-elf-objdump', '--disassemble']

def get_target_args(remnant):
    """Parse left over arguments"""
    parser = argparse.ArgumentParser(description='Get target specific args')

    parser.add_argument(
        '--cpu-mhz',
        type=int,
        default=1,
        help='Processor clock speed in MHz'
    )
    parser.add_argument(
        '--build-dir',
        type=str,
        default='bd_rvb',
        help='Path to build directory'
    )

    return parser.parse_args(remnant)

def build_benchmark_cmd(bench, args):
    """Construct the command to run the benchmark. "args" is a 
       namespace with target specific arguments"""

    bd_src = os.path.join(root, args.build_dir, 'src')
    bd_src = os.path.join(bd_src, bench)

    meminit_elf_cpy = copy.deepcopy(meminit_elf)
    meminit_elf_cpy.append(bench)
    meminit_elf_cpy.append(f"{bench}.bin")

    subprocess.run(meminit_elf_cpy, cwd=bd_src)
    print(f"Running {bench}")
    return [rvb_core, '--tohost-address', '2147487744' , '--max-sim-time', '429496729500', '--debug', '--notrace', bd_src + '/' + f'/{bench}.bin']

def decode_results(stdout_str, stderr_str):
    print("================stdout================")
    print(stdout_str);
    print("======================================")
    elapsed_cycles = int(re.findall(r"^Total Cycles: [0-9]+", stdout_str, re.MULTILINE)[0].split(" ")[2], 16)
    elapsed_instrs = int(re.findall(r"^Total Instructions: [0-9]+", stdout_str, re.MULTILINE)[0].split(" ")[2], 16)
    if elapsed_cycles == 0:
        elapsed_cycles = 1
        elapsed_instrs = 1

    global cpu_mhz
    print(f'Speed: {elapsed_cycles / cpu_mhz / 1000}')
    print(f'Instructions: {elapsed_instrs}')
    print(f'Clocks: {elapsed_cycles}')
    print(f'IPC: {elapsed_instrs / elapsed_cycles}')
    print('Returning result.\n')

    return elapsed_cycles / cpu_mhz / 1000
