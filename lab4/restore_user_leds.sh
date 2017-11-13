#!/bin/bash
#hell program to stop user LEDs   
# Stop user LED0  
cd /sys/class/leds  
cd beaglebone\:green\:usr0  
echo heartbeat > trigger  
echo "Restore user LED0" 
cd ../beaglebone\:green\:usr1  
echo heartbeat > trigger  
echo "Restore user LED1" 
cd ../beaglebone\:green\:usr2  
echo heartbeat > trigger  
echo "Restore user LED2" 
cd ../beaglebone\:green\:usr3  
echo heartbeat > trigger  
echo "Restore user LED3"
