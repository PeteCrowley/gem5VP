# Assemble the microbenchmark in bench.asm and link it to the executable bench
nasm -f elf32 -o microbenchmarks/object_files/bench.o microbenchmarks/src/bench.asm
ld -m elf_i386 -o microbenchmarks/bin/bench microbenchmarks/object_files/bench.o

# Run the microbenchmark in gem5
build/X86/gem5.opt --debug-flags=O3PipeView,Cache --debug-file=trace.out configs/deprecated/example/se.py --cpu-type=X86O3CPU --cmd=/home/rapiduser/gem5VP/microbenchmarks/bin/bench --caches --l1d_size=64kB --l1d_assoc=1 --l2_size=1MB --l2_assoc=1

# Generate the pipeline view
./util/o3-pipeview.py -c 500 -o pipeview.out --color m5out/trace.out

# Display it to the terminal, the last bit will be the interesting part
# grep -v '\.0 ADD_R_I' pipeview.out
cat pipeview.out
