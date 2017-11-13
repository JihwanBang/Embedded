#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#define NSEC_PER_SEC 1000000000L

#define timerdiff(a,b) ((float)((a)->tv_sec - (b)->tv_sec) + \
((float)((a)->tv_nsec - (b)->tv_nsec))/NSEC_PER_SEC)

static struct timespec prev = {.tv_sec=0,.tv_nsec=0};
static int count = 5;

void handler( signo )
{
  printf("handler start\n");
  struct timespec now;
  printf("handler : clock gettime\n");
  clock_gettime(CLOCK_MONOTONIC, &now);
  printf("[%d]Diff time:%lf\n", count, timerdiff(&now, &prev));
  prev = now;
  count --;
}

int main(int argc, char *argv[])
{
  int i = 0;
  timer_t t_id;

  struct itimerspec tim_spec = {.it_interval= {.tv_sec=1,.tv_nsec=10000},
  .it_value = {.tv_sec=2,.tv_nsec=10000}};

  struct sigaction act;
  sigset_t set;

  sigemptyset( &set );
  sigaddset( &set, SIGALRM );

  act.sa_flags = 0;
  act.sa_mask = set;
  act.sa_handler = &handler;
  printf("sigaction\n");
  sigaction( SIGALRM, &act, NULL );

  usleep(500000);
  printf("timer create!\n");
  if (timer_create(CLOCK_MONOTONIC, NULL, &t_id))
    perror("timer_create");
  printf("timer settime\n");
  if (timer_settime(t_id, 0, &tim_spec, NULL))
    perror("timer_settime");
  //printf("clock get time\n");
  //clock_gettime(CLOCK_MONOTONIC, &prev);
  printf("for loop\n");
  for (; ; )
  {
    // printf("count : %d",count);
    if(count == 0)
      break;
  }

  return 0;
}
