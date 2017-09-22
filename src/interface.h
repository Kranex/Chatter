#ifndef INTERFACE_H
#define INTERFACE_H

#define DISPSTOR 45
#define INPUSTOR 128
char inputStor[INPUSTOR], displayStor[DISPSTOR][INPUSTOR];
int inputStord, displayStord;

void updateInput();
void updateDisplay();
void initInterface();
void destroyInterface();
void displayNew(char n[]);
void displayPrint(char u[],char n[]);
char *getInput();
int QUIT_KEY;
#endif
