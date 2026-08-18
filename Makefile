set-executable:
	echo "export PATH=$PATH:/home/ano/Desktop/mini-runner/" > ~/.bashrc && source ~/.bashrc
compile:
	gcc *.c */*.c */*/*.c */*/*/*.c -march=native -fopenmp -pthread -lm -O3 -o mrn
run:
	./mrn
get-assembly:
	gcc -S -masm=intel *.c */*.c */*/*.c */*/*/*.c mrn.s