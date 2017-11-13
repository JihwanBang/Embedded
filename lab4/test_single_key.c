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

// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;

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

 
int main(void)
{
  char c;
  int echo;
 
  // Init termios: Disable buffered IO with arg 'echo'
  echo = 0;				// Disable echo
  init_termios(echo);

  // Test loop
  printf("  Test_single_key\n");
  printf("single key input until 'q' key\n");
  while (1) {
	c = getch();
	//printf("%c", c);
	printf("%c %2x ", c, c);
	fflush(stdout);

	if (c == 'q') break;
  }
  printf("  Quit!\n");
  fflush(stdout);

  // Reset termios
  reset_termios();

  return 0;
}


