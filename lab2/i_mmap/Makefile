all : m1,m2 

m1: 
	arm-linux-gnueabihf-gcc -o pushLEDmmap pushLEDmmap.c
m2: 
	arm-linux-gnueabihf-gcc -o userLEDmmap userLEDmmap.c
clean: 
	rm pushLEDmmap userLEDmmap
