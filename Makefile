set-executable:
	echo "export PATH=$PATH:/home/ano/Desktop/mini-runner/" > ~/.bashrc && source ~/.bashrc
compile:
	gcc *.c */*.c -o mrn
run:
	./mrn