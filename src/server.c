#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <pthread.h>

#include "chatterUtils.h"
#include "interface.h"

#define TRUE   1
#define FALSE  0
#define MAX_CLIENTS 30

char port[6];
int initalised = 0;
int users = 0;
pthread_t serverThread;

int prefix(const char *pre, const char *str)
{
  return strncmp(pre, str, strlen(pre)) == 0;
}
char* replaceChar(char* str, char find, char replace){
  char *current_pos = strchr(str,find);
  while (current_pos){
    *current_pos = replace;
    current_pos = strchr(current_pos,find);
  }
  return str;
}
void * server(void *args){
  int opt = TRUE;
  int master_socket, addrlen, new_socket, client_socket[MAX_CLIENTS], activity, valread, sd;
  int max_sd;
  char names[MAX_CLIENTS][16];
  struct sockaddr_in address;

  char buffer[128];  //data buffer of 1K

  //set of socket descriptors
  fd_set readfds;
  //initialise all client_socket[] to 0 so not checked
  for (int i = 0; i < MAX_CLIENTS; i++) {
    client_socket[i] = 0;
  }

  //create a master socket
  if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
    //  perror("socket failed");
    exit(EXIT_FAILURE);
  }

  //set master socket to allow multiple connections ,
  //this is just a good habit, it will work without this
  if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
  sizeof(opt)) < 0 ) {
    //perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  //type of socket created
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(atoi(port));

  //bind the socket to localhost port 8888
  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
    //perror("bind failed");
    exit(EXIT_FAILURE);
  }
  //  printf("Listener on port %d \n", PORT);

  //try to specify maximum of 3 pending connections for the master socket
  if (listen(master_socket, 3) < 0) {
    //perror("listen");
    exit(EXIT_FAILURE);
  }

  //accept the incoming connection
  addrlen = sizeof(address);
  //  puts("Waiting for connections ...");
  initalised = 1;
  while(TRUE) {
    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(master_socket, &readfds);
    max_sd = master_socket;

    //add child sockets to set
    for (int i = 0 ; i < MAX_CLIENTS ; i++) {
      //socket descriptor
      sd = client_socket[i];

      //if valid socket descriptor then add to read list
      if(sd > 0){
        FD_SET( sd , &readfds);
      }

      //highest file descriptor number, need it for the select function
      if(sd > max_sd){
        max_sd = sd;
      }
    }

    //wait for an activity on one of the sockets , timeout is NULL ,
    //so wait indefinitely
    activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

    if ((activity < 0) && (errno!=EINTR)) {
      //  printf("select error");
    }

    //If something happened on the master socket ,
    //then its an incoming connection
    if (FD_ISSET(master_socket, &readfds)) {
      if ((new_socket = accept(master_socket,
        (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
          //perror("accept");
          exit(EXIT_FAILURE);
        }

        //inform user of socket number - used in send and receive commands
        //printf("New connection , socket fd is %d , ip is : %s , port : %d\n",
        //new_socket , inet_ntoa(address.sin_addr) , ntohs
        //(address.sin_port));

        //send new connection greeting message
        //if( send(new_socket, message, strlen(message), 0) != strlen(message)) {
        //  //perror("send");
        //  }

        //  puts("Welcome message sent successfully");

        //add new socket to array of sockets
        for (int i = 0; i < MAX_CLIENTS; i++) {
          //if position is empty
          if( client_socket[i] == 0 ) {
            client_socket[i] = new_socket;
            snprintf(names[i], 16, "%s%d", "User", users);
            users++;
            //printf("Adding to list of sockets as %d\n" , i);

            break;
          }
        }
      }

      //else its some IO operation on some other socket
      for (int i = 0; i < MAX_CLIENTS; i++) {
        sd = client_socket[i];

        if (FD_ISSET( sd , &readfds)) {
          //Check if it was for closing , and also read the
          //incoming message
          if ((valread = read( sd , buffer, 128)) == 0) {
            //Somebody disconnected , get his details and print
            getpeername(sd , (struct sockaddr*)&address , \
            (socklen_t*)&addrlen);
            //  printf("User disconnected , ip %s , port %d \n" ,
            //  inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

            //Close the socket and mark as 0 in list for reuse
            close( sd );
            client_socket[i] = 0;
          } else {
            //Echo back the message that came in
            //set the string terminating NULL byte on the end
            //of the data read
            buffer[valread] = '\0';
            if(prefix("/name", buffer)){
              char name[16];
              strncpy(name, replaceChar(buffer, '\n', '\0') + 6, sizeof(name));
              int nameTaken = 0;
              for(int j = 0; j < MAX_CLIENTS; j++){
                if(!strcmp(name, names[j]) && strcmp(name, "")){
                  char msg[128+16];
                  snprintf(msg, 128+16, "<server> '%s' is already taken.\n", name);
                  send(client_socket[i], msg , strlen(msg) , 0 );
                  nameTaken = 1;
                }
              }
              for(int j = 0; j < MAX_CLIENTS && !nameTaken; j++){
                char msg[128+16];
                snprintf(msg, 128+16, "<server> '%s' is now '%s'\n", names[i], name);
                send(client_socket[j], msg , strlen(msg) , 0 );
              }
              if(!nameTaken){
                strcpy(names[i], name);
              }
            }else if(prefix("/tell", buffer)){
              int told = 0;
              char name[16];
              for(int j = 0; j < MAX_CLIENTS; j++){
                name[0] = '\0';
                for(int k = 0; k < 15 && buffer[6+k] != ' '; k++){
                  name[k] = buffer[6+k];
                  name[k+1] = '\0';
                }
                if(!strcmp(names[j], name) && strcmp(name, "")){
                  char msg[128];
                  strncpy(msg, buffer + 6 + strlen(name) + 1, strlen(buffer));
                  char namMsg[128+32];
                  sprintf(namMsg, "<%s->%s> %s", names[i], name, msg);
                  send(client_socket[j], namMsg, strlen(namMsg), 0);
                  send(client_socket[i], namMsg, strlen(namMsg), 0);
                  told = 1;
                  break;
                }
              }
              if(!told){
                char msg[128+16];
                snprintf(msg, 128+16, "<server> '%s' not found...\n", name);
                send(client_socket[i], msg , strlen(msg) , 0 );
              }
            }else{
              for(int j = 0; j < MAX_CLIENTS; j++){
                char msg[128+16];
                snprintf(msg, 128+16, "<%s> %s", names[i], buffer);
                send(client_socket[j], msg , strlen(msg) , 0 );
              }
            }
          }
        }
      }
    }
  }
  void initServer(char * p){
    strcpy(port, p);
    pthread_create(&serverThread, NULL, server, NULL);
    while(!initalised);
  }
