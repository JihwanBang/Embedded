/*
	Program test_single_key.c

	Get a key input without hitting Enter key
	using termios

    Modified from
http://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-linux
    Global old_termios and new_termios for efficient key inputs.

	Tested by	Byung Kook Kim,	July 7, 2016.
*/

#include <termios.h>
#include <stdio.h>
#include <unistd.h>		// read()
#include <pthread.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "Metronome_led.h"

#define NSEC_PER_SEC 1000000000

//#define timerdiff(a,b) ((float)((a)->tv_sec - (b)->tv_sec) + \
//((float)((a)->tv_nsec - (b)->tv_nsec))/NSEC_PER_SEC)

// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;
//volatile int keepgoing = 1;
volatile void *gpio_addr;
volatile unsigned int *gpio_datain;
volatile unsigned int *gpio_setdataout_addr;
volatile unsigned int *gpio_cleardataout_addr;
int timesig=3;
int run=0;
int tempo=90;
int s_time = 0;
int ntime = 1000*1000*1000 /90 * 60 /2; 
struct timespec prev = {.tv_sec=0, .tv_nsec=0};
  
  
// Initialize new terminal i/o settings 
void init_termios(int echo) 
{
  tcgetattr(0, &old_tio); 		// Grab old_tio terminal i/o setting 
  new_tio = old_tio; 			// Copy old_tio to new_tio
  new_tio.c_lflag &= ~ICANON; 	// disable buffered i/o 
  new_tio.c_lflag &= echo? ECHO : ~ECHO; 	// Set echo mode 
  if (tcsetattr(0, TCSANOW, &new_tio) < 0)  perror("tcsetattr ~ICANON");
								// Set new_tio terminal i/o setting
}


// Restore old terminal i/o settings 
void reset_termios(void) 
{
  tcsetattr(0, TCSANOW, &old_tio);
}


// Read one character without Enter key: Blocking
char getch(void) 
{
  char ch = 0;

  //if (read(0, &ch, 1) < 0)  perror ("read()");		// Read one character
  while (read(0, &ch, 1) <0 ){

  } 
  return ch;
}


void off_tempo(void){
	*gpio_cleardataout_addr = USR0_LED+USR1_LED+USR2_LED;
	printf(".");
	fflush(stdout);
}

void on_tempo(int timesig_curr){
	if (timesig_curr == 0){
		printf("1");
		fflush(stdout);
		*gpio_setdataout_addr = USR0_LED;
	}
	if (timesig_curr == 1){
		printf("3");
		fflush(stdout);
		*gpio_setdataout_addr = USR0_LED+USR1_LED;
	}	
	if (timesig_curr == 2){
		printf("7");
		fflush(stdout);
		*gpio_setdataout_addr = USR0_LED+USR1_LED+USR2_LED;
	}
	
	
}

void handler(int signo){
  
  	static int count;
	
  	gpio_datain            = gpio_addr + GPIO_DATAIN;
  	gpio_setdataout_addr   = gpio_addr + GPIO_SETDATAOUT;
  	gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;
  	if (run == 0){
		fflush(stdout);
	}
	else{
		switch(timesig){
		  case 1: 
		  	if (count == 0)	on_tempo(2);
		    else if (count % 2 == 1) off_tempo();
		    else on_tempo(0);
			count = (count+1)%4;
			break;
		  
		  case 2: 
			if (count == 0)	on_tempo(2);
			else if (count % 2 == 1) off_tempo();
			else on_tempo(0);
			count = (count+1)%6;
			break;

		  case 3: 
			if (count == 0)	on_tempo(2);
			else if (count == 4) on_tempo(1);
			else if (count % 2 == 1) off_tempo();
			else on_tempo(0);
			count = (count+1)%8;
			break;

		  case 4: 
			if (count == 0) on_tempo(2);
			else if (count == 6) on_tempo(1);
			else if (count % 2 == 1) off_tempo();
			else on_tempo(0);

			count = (count+1)%12;
			break;
		}
				
	}
  

}
int key_hit(){
  struct timeval tv ={0L, 0L};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(0, &fds);
  return select(1, &fds, NULL, NULL, &tv);
}


int main(void)
{
  char c;
  int echo;
  int fd = open("/dev/mem", O_RDWR);
  char* comp[4] = {"2/4", "3/4", "4/4", "6/8"};
  timer_t t_id;
  
  gpio_addr = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                        GPIO1_BASE);
  
  if(gpio_addr == MAP_FAILED){
	printf("unable to map GPIO\n");
	exit(1);
  }
  // Init termios: Disable buffered IO with arg 'echo'
  echo = 0;				// Disable echo
  init_termios(echo);

  struct itimerspec tim_spec = {.it_interval={.tv_sec=0, .tv_nsec=ntime},.it_value={.tv_sec=1,.tv_nsec=100000}};
  struct sigaction act;
  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGALRM);
  act.sa_flags = 0;
  act.sa_mask = set;
  act.sa_handler = &handler;

  sigaction(SIGALRM, &act, NULL);

  if (timer_create(CLOCK_MONOTONIC, NULL, &t_id))
    perror("timer_create");

  if (timer_settime(t_id, 0, &tim_spec, NULL))
    perror("timer_settime");

  // Test loop
  printf("Menu for Metronome_HRT : \n");
  printf("'z': Time signature 2/4 > 3/4 > 4/4 > 6/8 > 2/4 ... \n");
  printf("'c': Dec tempo 			Dec tempo by 5\n");
  printf("'b': Inc tempo 			Inc tempo by 5\n");
  printf("'m': Start/Stop 		Toggles start and stop\n");
  printf("'q': Quit this program\n");

  while(1){
	//non blocking module 
	while(!key_hit()){
	  usleep(250);
	}

	c= getch();
	
	if (c == 'q') break;
	if (c == 'z'){
		timesig = timesig%4+1;
	}
	if (c == 'c'){
		tempo -= 5;
		if (tempo < 30) 
			tempo = 30;

		ntime = 1000000000 / tempo * 60 /2; 
		s_time = ntime / 1000000000;
		ntime = ntime % 1000000000;
		// printf("ntime: %d",ntime);
	}	
	if (c == 'b'){
		tempo += 5;
		if (tempo >200) 
			tempo = 200;
		ntime = 1000000000 / tempo * 60 /2;
		s_time = ntime / 1000000000;
		ntime = ntime % 1000000000;
	}
	if (c == 'm'){
		if (run == 1) 
			run = 0;

		else 
			run = 1;
	}

	
	if (run){
		tim_spec.it_interval.tv_nsec = ntime;
		tim_spec.it_interval.tv_sec =	s_time;
	}
		
	else{
		tim_spec.it_interval.tv_nsec = 500000000;
		tim_spec.it_interval.tv_sec = 0;
	}
	if (timer_settime(t_id, 0, &tim_spec, NULL))
    	perror("timer_settime");
	printf("Key %c: TimeSig %s, Tempo %d, Run %d.\n", c, comp[timesig-1], tempo , run);
	fflush(stdout);	
  }
  printf("  Quit!\n");
  fflush(stdout);
  munmap((void *)gpio_addr, GPIO_SIZE);
  close(fd);
  // Reset termios
  reset_termios();

  return 0;
}


