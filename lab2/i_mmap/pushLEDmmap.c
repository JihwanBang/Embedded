#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>  
#include <signal.h>    // Defines signal-handling functions (i.e. trap Ctrl-C) 
#include <unistd.h>        // close() 
#include "pushLEDmmap.h" 
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
    printf("Mapping %X - %X (size: %X)\n", GPIO0_START_ADDR, GPIO0_END_ADDR,  
                                           GPIO0_SIZE); 
    gpio_addr = mmap(0, GPIO0_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,  
                        GPIO0_START_ADDR); 
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
    while(keepgoing) { 
        if(*gpio_datain & GPIO_07) { 
            *gpio_setdataout_addr= GPIO_31; 
        } else { 
            *gpio_cleardataout_addr = GPIO_31; 
        } 
        //usleep(1); 
    } 
    munmap((void *)gpio_addr, GPIO0_SIZE); 
    close(fd); 
    return 0; 
} 
