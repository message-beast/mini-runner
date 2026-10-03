COMPILER_AND_FLAGS = gcc -c
compile-close:
	$(COMPILER_AND_FLAGS) close.s -o close.o
compile-fstatat:
	$(COMPILER_AND_FLAGS) fstatat.s -o fstat.o
compile-getdents64:
	$(COMPILER_AND_FLAGS) getdents64.s -o getdents64.o
compile-openat:
	$(COMPILER_AND_FLAGS) openat.s -o openat.o
compile-rmdir:
	$(COMPILER_AND_FLAGS) rmdir.s -o rmdir.o
compile-unlinkat:
	$(COMPILER_AND_FLAGS) unlinkat.s -o unlinkat.o
compile-all:
	make compile-close && make compile-fstatat && make compil-getdents64 && make compile-openat && make compile-rmdir && make compile-unlinkat