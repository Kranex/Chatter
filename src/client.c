#include "client.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include "interface.h"

int sock, disconnected = 0;
pthread_t listenerThread;

void initSockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port) {
  struct hostent *hostinfo;

  name->sin_family = AF_INET;
  name->sin_port = htons (port);
  hostinfo = gethostbyname (hostname);

  if (hostinfo == NULL) {
    //fprintf (stderr, "Unknown host %s.\n", hostname);
    exit (EXIT_FAILURE);
  }
  name->sin_addr = *(struct in_addr *) hostinfo->h_addr_list[0];
}

void write_to_server (char msg[128]) {
  if (disconnected) {
      strcpy(msg, "<error> Disconnected from server. please '/quit'.");
      displayNew(msg);
      return;
  }
  write (sock, msg, strlen (msg) + 1);
}

void * listener(){
  while(1){
    char msg[128];

    for(int i = 0; i < sizeof(msg); i++){
          if(read(sock, &msg[i], 1) <= 0){
            strcpy(msg, "<error> Disconnected from server. please '/quit'.");
            displayNew(msg);
            disconnected = 1;
            return NULL;
          };
          if(msg[i] == '\n'){
            msg[i] = '\0';
            break;
          }

    }
    displayNew(msg);
  }
}

int initClient(char ip[15], char port[6]){
  // JOIN SERVER.
  char joinMsg[64];
  sprintf(joinMsg, "Joining %s:%s...", ip, port);
  displayPrint("<client>", joinMsg);
  struct sockaddr_in servername;

/* Create the socket. */
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    //perror ("socket (client)");
    return 0;
  }

  initSockaddr (&servername, ip, atoi(port));
  if (0 > connect (sock, (struct sockaddr *) &servername, sizeof (servername))) {
    //perror ("connect (client)");
    return 0;
  }
  displayPrint("<client>", "Connected!");
  pthread_create(&listenerThread, NULL, listener, NULL);
  return 1;
}


void killClient(){
    pthread_cancel(listenerThread);
    close (sock);
}
