compile:
	gcc *.c */*.c */*/*.c */*/*/*.c syscalls/x86/64-bit/*.o -fopenmp -pthread -lm -O2 -mavx512f -mavx2 -mavx -msse4.2 -msse4.1 -mssse3 -msse3 -msse2 -msse -mfma -o mrn
run:
	./mrn
get-assembly:
	gcc -S -masm=intel *.c */*.c */*/*.c */*/*/*.c mrn.s