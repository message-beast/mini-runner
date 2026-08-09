set-executable:
	echo "export PATH=$PATH:/home/ano/Desktop/mini-runner/" > ~/.bashrc && source ~/.bashrc
compile:
	gcc -oo *.c */*.c */*/*.c */*/*/*.c -march=native -fopenmp -pthread -o mrn
run:
	./mrn
get-assembly:
	gcc -S *.c */*.c */*/*.c */*/*/*.c mrn.s