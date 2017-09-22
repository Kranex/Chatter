#include <stdlib.h>
#include <string.h>
#include "chatterUtils.h"

#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

void sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}
char **getCommand(char *inp){
  int args = 1;
  int length = strlen(inp);
  for(int i = 0; i < length-1; i++){
    if(inp[i] == ' ' && inp[i+1] != ' ' && inp[i+1] != '\0'){
      args++;
    }
  }
  int nxt = 0;
  int arg = 0;

  char **com = malloc(args * sizeof(char *));
  com[0] = malloc(length * sizeof(char));
  for(int i = 0; i < length; i++){
    if(inp[i] == '\0'){
      com[arg][nxt] = '\0';
      break;
    }
    if(i > 0 && inp[i] == ' '){
      if(inp[i-1] != ' '){
        com[arg][nxt] = '\0';
        nxt = 0;
        if(arg < args){
          arg++;
          com[arg] = malloc(length * sizeof(char));
        }
      }
      continue;
    }
    com[arg][nxt] = inp[i];
    nxt++;
  }

  return com;
}
