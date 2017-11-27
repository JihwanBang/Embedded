all: server client 

server: 
	arm-linux-gnueabihf-gcc -o metro_server_bone metro_server_bone.c -lrt

client:
	gcc -o metro_client metro_client.c -pthread
