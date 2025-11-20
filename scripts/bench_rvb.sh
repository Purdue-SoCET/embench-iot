./build_all.py --arch riscv32 --chip rvb --board rvb --builddir bd_rvb --clean
./benchmark_speed.py --target-module run_rvb --timeout 3600 --builddir bd_rvb --sim-parallel --absolute
