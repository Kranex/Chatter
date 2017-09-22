#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interface.h"
#include "networker.h"
#include "chatterUtils.h"
int main(int argc, char *argv[]){
  initInterface();
  displayNew("////////////////////////////////////////////////////");
  displayNew("// Welcome to Oli's Pizza Delivery Service v0.0.1 //");
  displayNew("// Arguably the worst instant messenger there is! //");
  displayNew("////////////////////////////////////////////////////");

  char ip[16];
  char port[6];
  int cmdAccepted = 0;
  char **command, *str;
  displayPrint("<info>", "Type '/join <ip> <port>' to join a network.");
  displayPrint("<info>", "Type '/host <port>' to host a network.");
  displayPrint("<info>", "Type '/help' for more information.");
  while(!cmdAccepted){
    str = getInput();
    command = getCommand(str);
    if(!strcmp(command[0], "/quit")){
      free(str);
      free(command);
      destroyInterface();
      return 0;
    }
    if(!strcmp(command[0], "/join")){
      if(sizeof(command) < 3){
        displayPrint("<info>", "Usage: '/join <ip> <port> to join a network.");
        continue;
      }
      for(int i = 0; i < 16; i++){
        ip[i] = command[1][i];
        if(command[1][i] == '\0'){
          break;
        }
      }
      for(int i = 0; i < 6; i++){
        port[i] = command[2][i];
        if(command[2][i] == '\0'){
          break;
        }
      }
      cmdAccepted = startClient(ip, port);
      if(!cmdAccepted){
        displayPrint("<error>", "Error connecting to server...");
      }
      continue;
    }else if(!strcmp(command[0],"/host")){
      if(sizeof(command) < 2){
        displayPrint("<info>", "Type '/host <port>' to host a network.");
        continue;
      }
      strcpy(ip, "127.0.0.1");
      for(int i = 0; i < 6; i++){
        port[i] = command[1][i];
        if(command[1][i] == '\0'){
          break;
        }
      }
      // START SERVER;
      startServer(port);
      cmdAccepted = startClient(ip, port);
      continue;
    }else{
      char err[128];
      sprintf(err, "Unknown command: '%s'.", str);
      displayPrint("<error>", err);
      displayPrint("<info>", "Type '/help' for more information.");
    }
  }
  free(str);
  free(command);
  char inp[128];
   str = getInput();
  strcpy(inp, str);
  free(str);
  while(strcmp(inp, "/quit")){
    int length = strlen(inp);
    inp[length] = '\n';
    inp[length+1] = '\0';
    sendMesg(inp);
    str = getInput();
    strcpy(inp, str);
    free(str);
  }
  destroyInterface();
  return 0;
}
