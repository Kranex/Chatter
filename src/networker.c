#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close


#include "interface.h"
#include "server.h"
#include "client.h"

void startServer(char * port){
  char str[128];
  sprintf(str,"Starting Server on port: %s...", port);
  displayPrint("<server>", str);
  initServer(port);
}

int startClient(char * ip, char * port){
  return initClient(ip, port);
}

void sendMesg(char msg[128]){
  write_to_server(msg);
}
