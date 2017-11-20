/*
	Program test_single_key_nb.c

	Test checking single key without enter
	with non-blocking mode
	using termios

    Modified from
http://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-linux
    Global old_termios and new_termios for efficient key inputs.

	Tested by	Byung Kook Kim,	Aug. 10, 2016.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <termios.h>
#include <unistd.h>		// read()
#include <sys/select.h>

// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;

char waitchar[4] = { '|', '/', '-', '\\'  };

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


int key_hit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}


// Read one character without Enter key: Blocking
char getch(void) 
{
  char ch = 0;

  if (read(0, &ch, 1) < 0)  usleep(250)//perror ("read()");		// Read one character
 
  return ch;
}

 
int main(void)
{
  char c;
  int i;
  int echo;
 
  // Init termios: Disable buffered IO with arg 'echo'
  echo = 0;				// Disable echo
  init_termios(echo);
  i = 0;

  // Test loop
  printf("  Test_single_key_nb\n");
  printf("single key input in non-blocking mode until 'q' key\n");
  while (1) {
	while (!key_hit()) {
	  i = ++i % 4;
	  printf("%c", waitchar[i]);
	  fflush(stdout);
	  usleep(250000);		// 0.25 s
	}
	c = getch();
	printf("%c", c);
	fflush(stdout);

	if (c == 'q') break;
  }
  printf("  Quit!\n");
  fflush(stdout);

  // Reset termios
  reset_termios();

  return 0;
}


