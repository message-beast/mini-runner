CPU_FLAGS = -mavx512f -mavx2 -mavx -msse4.2 -msse4.1 -mssse3 -msse3 -msse2 -msse -mfma
SOURCE_CODE = *.c */*.c */*/*.c */*/*/*.c syscalls/x86/64-bit/*.o
SOURCE_CODE_TW = *.c */*.c */*/*.c */*/*/*.c syscalls/x86/32-bit/*.o
CPP_GARBAGE_DISABLING_FLAGS = -fno-exceptions -fno-asynchronous-unwind-tables -fno-unwind-tables -fomit-frame-pointer
GEN_OPT_FLAGS = -fopenmp -pthread -lm -O2
ASM_TYPE = -masm=intel
EXECUTABLE_PATH = /usr/lib/mrn/bin/mrn

compile:
	gcc $(SOURCE_CODE) $(GEN_OPT_FLAGS) $(CPP_GARBAGE_DISABLING_FLAGS) -o $(EXECUTABLE_PATH)
compile-for-32-bit:
	gcc $(SOURCE_CODE_TW) -m32 $(GEN_OPT_FLAGS) $(CPP_GARBAGE_DISABLING_FLAGS) -o $(EXECUTABLE_PATH) 
run:
	./mrn
get-assembly:
	gcc $(SOURCE_CODE) $(GEN_OPT_FLAGS) $(CPU_FLAGS) $(CPP_GARBAGE_DISABLING_FLAGS) $(ASM_TYPE) -S

remove-assembly:
	rm -rf *.s

