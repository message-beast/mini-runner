compile-close:
	nasm -felf64 close.s -o close.o
compile-fstatat:
	nasm -felf64 fstatat.s -o fstat.o
compile-getdents64:
	nasm -felf64 getdents64.s -o getdents64.o
compile-openat:
	nasm -felf64 openat.s -o openat.o
compile-rmdir:
	nasm -felf64 rmdir.s -o rmdir.o
compile-unlinkat:
	nasm -felf64 unlinkat.s -o unlinkat.o
compile-all:
	make compile-close && make compile-fstatat && make compil-getdents64 && make compile-openat && make compile-rmdir && make compile-unlinkat