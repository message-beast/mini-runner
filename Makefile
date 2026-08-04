set-executable:
	echo "export PATH=$PATH:/home/ano/Desktop/mini-runner/" > ~/.bashrc && source ~/.bashrc
compile:
	gcc *.c */*.c */*/*.c */*/*/*.c -pthread -o mrn
run:
	./mrn