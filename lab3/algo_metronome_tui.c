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

// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;
volatile int keepgoing = 1;
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

  if (read(0, &ch, 1) < 0)  perror ("read()");		// Read one character
 
  return ch;
}

void signal_handler(int sig) { 
    printf( "\nCtrl-C pressed, cleaning up and exiting...\n" ); 
       keepgoing = 0; 
}
 
typedef struct metronome{
	volatile void *gpio_addr;
	int timesig;
	int tempo;
	int run;
} metro;

void one_step_tempo(int time, int strength, volatile unsigned int *set, volatile unsigned int *clear){
	if (strength==3){
		printf("7");
		fflush(stdout);
		*set = USR0_LED+USR1_LED+USR2_LED;
		usleep(time/2);
		*clear = USR0_LED+USR1_LED+USR2_LED;
		usleep(time/2);
	}
	if (strength == 2){
		printf("3");
		fflush(stdout);
		*set = USR0_LED+USR1_LED;
		usleep(time/2);
		*clear = USR0_LED+USR1_LED;
		usleep(time/2);
	}
	if (strength == 1){
		printf("1");
		fflush(stdout);
		*set = USR0_LED;
		usleep(time/2);
		*clear = USR0_LED;
		usleep(time/2);
	}
}
void* thread_main(void *met){
  metro *th_met = (metro *)met;
  volatile void *gpio_addr = (*th_met).gpio_addr;
  volatile unsigned int *gpio_datain;
  volatile unsigned int *gpio_setdataout_addr;
  volatile unsigned int *gpio_cleardataout_addr;

  
  //int utime_value = 60*1000000/(*th_met).tempo;
  int two_four[2] = {3,1};
  int three_four[3] = {3,1,1};
  int four_four[4] = {3,1,2,1};
  int six_eight[6] = {3,1,1,2,1,1};
 
  gpio_datain            = gpio_addr + GPIO_DATAIN;
  gpio_setdataout_addr   = gpio_addr + GPIO_SETDATAOUT;
  gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;
  while(1){
	int utime = 60*1000000/(*th_met).tempo;
	if ((*th_met).run == 0){
		fflush(stdout);
		usleep(100000);
	}
  	else{
		switch((*th_met).timesig){
		  case 1: for (int i = 0; i < 2;i++){
				one_step_tempo(utime, two_four[i], gpio_setdataout_addr, gpio_cleardataout_addr);
			  } 
			  break;
		  case 2: for (int i = 0; i < 3; i++){
				one_step_tempo(utime, three_four[i], gpio_setdataout_addr, gpio_cleardataout_addr);
			  }
			  break;
		  case 3: for (int i = 0; i< 4; i++){
				one_step_tempo(utime, four_four[i], gpio_setdataout_addr, gpio_cleardataout_addr);
			  }
			  break; 
		  case 4: for (int i = 0 ; i<6; i++){
				one_step_tempo(utime, six_eight[i], gpio_setdataout_addr, gpio_cleardataout_addr);
			  }
			  break;
		}			
	}
  }

}


int main(void)
{
  char c;
  int echo;
  signal(SIGINT, signal_handler);
  int fd = open("/dev/mem", O_RDWR);
  char* comp[4] = {"2/4", "3/4", "4/4", "6/8"};
  metro met;
  met.timesig=3;
  met.tempo=90;
  met.run=0;
  met.gpio_addr = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                        GPIO1_BASE);
  if(met.gpio_addr == MAP_FAILED){
	printf("unable to map GPIO\n");
	exit(1);
  }
  pthread_t thread;
  // Init termios: Disable buffered IO with arg 'echo'
  echo = 0;				// Disable echo
  init_termios(echo);

  // Test loop
  printf("Menu for Metronome_TUI : \n");
  printf("'z': Time signature 2/4 > 3/4 > 4/4 > 6/8 > 2/4 ... \n");
  printf("'c': Dec tempo 			Dec tempo by 5\n");
  printf("'b': Inc tempo 			Inc tempo by 5\n");
  printf("'m': Start/Stop 		Toggles start and stop\n");
  printf("'q': Quit this program\n");
  int status = pthread_create(&thread, NULL, &thread_main, (void *)&met);
	if (status != 0)
		printf("cannot create thread\n");
	else 
		printf("create successfully\n");

  while (keepgoing) {
	c= getch();
	//printf("%c", c);
	//fflush(stdout);
	/*
	int status = pthread_create(&thread, NULL, &thread_main, (void *)&met);
	c = getch();
	if (status != 0)
		printf("cannot create thread\n");
	else 
		printf("create successfully\n");
	*/
	
	if (c == 'q') break;
	if (c == 'z'){
		met.timesig = met.timesig%4+1;
	}
	if (c == 'c'){
		met.tempo -= 5;
		if (met.tempo < 30) 
			met.tempo = 30;
	}	
	if (c == 'b'){
		met.tempo += 5;
		if (met.tempo >200) 
			met.tempo = 200;
	}
	if (c == 'm'){
		if (met.run == 1) 
			met.run = 0;
		else 
			met.run = 1;
	}

	printf("Key %c: TimeSig %s, Tempo %d, Run %d.\n", c, comp[met.timesig-1], met.tempo , met.run);
	fflush(stdout);	
	//pthread_cancel(thread);
  }
  pthread_cancel(thread);
  printf("  Quit!\n");
  fflush(stdout);
  munmap((void *)met.gpio_addr, GPIO_SIZE);
  close(fd);
  // Reset termios
  reset_termios();

  return 0;
}


