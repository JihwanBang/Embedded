#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <pthread.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <termios.h>
#define LISTEN_PORT 4950
#define MAXDATASIZE 1

int timesig=3;
int run=0;
int tempo=90;
char* comp[4] = {"2/4", "3/4", "4/4", "6/8"};



static struct termios old_tio;
static struct termios new_tio;
char getch(void){
	char ch = 0;
	if (read(0, &ch, 1) <0 ) perror ("read()");
	return ch;
}

void init_termios(int echo) 
{
  tcgetattr(0, &old_tio);               // Grab old_tio terminal i/o setting 
  new_tio = old_tio;                    // Copy old_tio to new_tio
  new_tio.c_lflag &= ~ICANON;   // disable buffered i/o 
  new_tio.c_lflag &= echo? ECHO : ~ECHO;        // Set echo mode 
  if (tcsetattr(0, TCSANOW, &new_tio) < 0)  perror("tcsetattr ~ICANON");
}


void reset_termios(void)
{
	tcsetattr(0, TCSANOW, &old_tio);
}

void *thread_main(void* arg){
	int *clnt_sock = (int *)arg;
	int recv_size;
	char buf[MAXDATASIZE];
	printf("thread start!\n");
	while(1){
		recv_size = recv(*clnt_sock,buf, MAXDATASIZE,0);
		if (recv_size <0)
			perror("recv error\n");
		printf("%s", buf);
		fflush(stdout);
	} 
}

int main(void){


	struct sockaddr_in serv_addr;
	int clnt_sock;
	char c;
	pthread_t thread;
	int echo=0;
	init_termios(echo);
	char message[40];
	clnt_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (clnt_sock == -1) 
		perror("could not create socket");

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port= htons(LISTEN_PORT);

	if(inet_pton(AF_INET, "192.168.7.2", &serv_addr.sin_addr) <= 0) 
		perror("Invalid Address!");

	if (connect(clnt_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0)
		perror("connection failed!!\n");


	printf("connect!!!\n");
	printf("Menu for Metronome_HRT : \n");
	printf("'z': Time signature 2/4 > 3/4 > 4/4 > 6/8 > 2/4 ... \n");
	printf("'c': Dec tempo                        Dec tempo by 5\n");
	printf("'b': Inc tempo                        Inc tempo by 5\n");
	printf("'m': Start/Stop               Toggles start and stop\n");
	printf("'q': Quit this program\n");
	pthread_create(&thread, NULL, &thread_main, (void *)&clnt_sock);
	while (1){
		c = getch();
		//printf("get %c\n",c);
		if (c == 'z') {
		  timesig = timesig%4 +1;
		}
		if (c == 'c') {
		  tempo -= 5;
		  if (tempo < 30)
		    tempo = 30;
		  //ntime = 1000000000 / tempo * 60 /2;
		  //s_time = ntime / 1000000000;
		  //ntime = ntime % 1000000000;
		}
		if (c == 'b') {
		  tempo += 5;
		  if (tempo > 200)
		    tempo = 200;
		  //ntime = 1000000000 / tempo * 60 /2;
		  //s_time = ntime / 1000000000;
		  //ntime = ntime % 1000000000;
		}
		if (c == 'm') {
		  if (run == 1)
		    run = 0;
		  else
		    run = 1;
		}
		if (run){
		  sprintf(message, "TimeSig %s, Tempo %d, Start", comp[timesig-1], tempo);
		}
		else{
		  sprintf(message, "Stop");
		}

		if (c == 'q') {
		  sprintf(message, "Quit");
		}
		printf ("[send]%s\n", message);
		if (send(clnt_sock, message, 40, 0) <0)
			perror("send error!");
		if (c == 'q') break;
	}
	printf("close the socket!!\n");
	reset_termios();
	close(clnt_sock);
	 	
}


 
