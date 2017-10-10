#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>  
#include <signal.h>    // Defines signal-handling functions (i.e. trap Ctrl-C) 
#include <unistd.h>        // close() 
#include "Metronome_led.h" 
#define CLOCK 1000*500

// Global variables 
volatile int keepgoing = 1;    // Set to 0 when Ctrl-c is pressed 
// Callback called when SIGINT is sent to the process (Ctrl-C) 
void signal_handler(int sig) { 
    printf( "\nCtrl-C pressed, cleaning up and exiting...\n" ); 
       keepgoing = 0; 
} 

 

int main(int argc, char *argv[]) { 
    volatile void *gpio_addr; 
    volatile unsigned int *gpio_datain; 
    volatile unsigned int *gpio_setdataout_addr; 
    volatile unsigned int *gpio_cleardataout_addr; 
    // Set the signal callback for Ctrl-C 
    signal(SIGINT, signal_handler); 
    int fd = open("/dev/mem", O_RDWR); 
    
    gpio_addr = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,  
                        GPIO1_BASE);     
    
    gpio_datain            = gpio_addr + GPIO_DATAIN; 
    gpio_setdataout_addr   = gpio_addr + GPIO_SETDATAOUT; 
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT; 

    if(gpio_addr == MAP_FAILED) { 
        printf("Unable to map GPIO\n"); 
        exit(1); 
    } 
    printf("GPIO mapped to %p\n", gpio_addr); 
    printf("GPIO SETDATAOUTADDR mapped to %p\n", gpio_setdataout_addr); 
    printf("GPIO CLEARDATAOUT mapped to %p\n", gpio_cleardataout_addr); 
    printf("Start copying GPIO_07 to GPIO_31\n"); 
    /*while(keepgoing) { 
        if(*gpio0_datain & GPIO_07) { 
            *gpio0_setdataout_addr=  
        } else { 
            *gpio_cleardataout_addr = GPIO_31; 
        } 
        //usleep(1); 
    } */
    while(keepgoing){
	*gpio_setdataout_addr = USR0_LED+USR1_LED+USR2_LED;
	usleep(CLOCK);
	*gpio_cleardataout_addr = USR0_LED+USR1_LED+USR2_LED;
	usleep(CLOCK);
	*gpio_setdataout_addr = USR0_LED;
	usleep(CLOCK);
	*gpio_cleardataout_addr = USR0_LED;
	usleep(CLOCK);
	*gpio_setdataout_addr = USR0_LED;
	usleep(CLOCK);
	*gpio_cleardataout_addr = USR0_LED;
	usleep(CLOCK);
	*gpio_setdataout_addr = USR0_LED+USR1_LED;
	usleep(CLOCK);
	*gpio_cleardataout_addr = USR0_LED+USR1_LED;
	usleep(CLOCK);
	*gpio_setdataout_addr = USR0_LED;
	usleep(CLOCK);
	*gpio_cleardataout_addr = USR0_LED;
	usleep(CLOCK);
	*gpio_setdataout_addr = USR0_LED;
	usleep(CLOCK);
	*gpio_cleardataout_addr = USR0_LED;
	usleep(CLOCK);
    }
    munmap((void *)gpio_addr, GPIO_SIZE); 
    close(fd); 
    return 0; 
} 

