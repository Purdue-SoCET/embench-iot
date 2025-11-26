./build_all.py --arch riscv32 --chip rvb_vm --board rvb --builddir bd_rvb_vm --clean
./benchmark_speed.py --target-module run_rvb_vm --timeout 3600 --builddir bd_rvb_vm --sim-parallel --absolute
