#ifndef NETWORKER_H
#define NETWORKER_H
void startServer(char * port);
int startClient(char * ip, char * port);
void sendMesg(char msg[128]);
#endif
