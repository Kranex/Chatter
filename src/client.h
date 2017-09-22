#ifndef CLIENT_H
#define CLIENT_H
void write_to_server(char msg[128]);
int initClient(char ip[16],char port[6]);
void killClient();
#endif
