CPU_FLAGS = -mavx512f -mavx2 -mavx -msse4.2 -msse4.1 -mssse3 -msse3 -msse2 -msse -mfma


ARCH_DIR = arch/*.c
BASE_DIR = base/*.c
CHECKS_FOR_PR_DIR = checks_for_pr/*.c
DAEMON_OPS_DIR =  daemon_ops/*.c
DAEMON_UTILS_DIR = daemon_utils/*.c
ENV_MAN_DIR = env_man/*.c
EXCEPTIONS_DIR = exceptions/*/*.c exceptions/*/*/*.c
FILE_SYS_OPS_DIR = file_sys_ops/*.c
FINI_DIR = fini/*.c
INIT_DIR = init/*.c
IO_MAN_DIR = io_man/*.c io_man/*/*.c io_man/*/*/*.c
JOB_FORMAT_DIR = job_format/*.c
JOB_OPS_DIR =  job_ops/*.c
JOB_RES_MAN_DIR = job_res_man/*.c
OPS_DIR = ops/*.c
RES_FORMAT_DIR = res_format/*.c
RES_MAN_DIR = res_man/*/*.c res_man/*/*/*.c 
SYNC_DIR = sync/*.c
TIME_SHIFT_DIR = time_shift/*.c
UTILS_OPS_DIR = utils_ops/*.c


C_SOURCE_CODE = *.c $(ARCH_DIR) $(BASE_DIR) $(CHECKS_FOR_PR_DIR) $(DAEMON_OPS_DIR) $(DAEMON_UTILS_DIR) $(ENV_MAN_DIR) $(EXCEPTIONS_DIR) $(FILE_SYS_OPS_DIR) $(FINI_DIR) $(INIT_DIR) $(IO_MAN_DIR) $(JOB_FORMAT_DIR) $(JOB_OPS_DIR) $(JOB_RES_MAN_DIR) $(OPS_DIR) $(RES_FORMAT_DIR) $(RES_MAN_DIR) $(SYNC_DIR) $(TIME_SHIFT_DIR) $(UTILS_OPS_DIR)

SOURCE_CODE_x86_64 = $(C_SOURCE_CODE) syscalls/x86/64-bit/*.o
SOURCE_CODE_x86_32 = $(C_SOURCE_CODE) syscalls/x86/32-bit/*.o
SOURCE_CODE_ARM_32 = $(C_SOURCE_CODE) syscalls/arm/32-bit/*.o
SOURCE_CODE_ARM_64 = $(C_SOURCE_CODE) syscalls/x86/64-bit/*.o
CPP_GARBAGE_DISABLING_FLAGS = -fno-exceptions -fno-asynchronous-unwind-tables -fno-unwind-tables -fomit-frame-pointer
GEN_OPT_FLAGS = -fopenmp -pthread -lm -O2
ASM_TYPE = -masm=intel
EXECUTABLE_PATH = /usr/lib/mrn/bin/mrn

compile:
	gcc $(SOURCE_CODE_x86_64) $(GEN_OPT_FLAGS) $(CPP_GARBAGE_DISABLING_FLAGS) -o $(EXECUTABLE_PATH)
compile-x86_32:
	gcc $(SOURCE_CODE_x86_32) $(GEN_OPT_FLAGS) $(CPP_GARBAGE_DISABLING_FLAGS) -o $(EXECUTABLE_PATH) 
compile-arm_64:
	gcc $(SOURCE_CODE_ARM_64) $(GEN_OPT_FLAGS) $(CPP_GARBAGE_DISABLING_FLAGS) -o $(EXECUTABLE_PATH) 
compile-arm_32:
	gcc $(SOURCE_CODE_ARM_32) $(GEN_OPT_FLAGS) $(CPP_GARBAGE_DISABLING_FLAGS) -o $(EXECUTABLE_PATH) 
run:
	./mrn
get-assembly:
	gcc $(SOURCE_CODE) $(GEN_OPT_FLAGS) $(CPU_FLAGS) $(CPP_GARBAGE_DISABLING_FLAGS) $(ASM_TYPE) -S

remove-assembly:
	rm -rf *.s

