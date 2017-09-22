#include <ncurses.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "interface.h"
#include "chatterUtils.h"

WINDOW *create_newwin(int height, int width, int starty, int startx);

pthread_t updateThread;
WINDOW *inputWin, *displayWin;

int width, displayHeight, inputHeight = 3;

void display(){
  displayWin = create_newwin(displayHeight, width, 0, 0);
}

void input(){
  inputWin = create_newwin(inputHeight, width, displayHeight, 0);
}

void updateInput(){
  werase(inputWin);
  box(inputWin, 0 , 0);
  mvwprintw(inputWin, 1, 1, ">%s", inputStor);
  wrefresh(inputWin);
}
void updateDisplay(){
  werase(displayWin);
  box(displayWin, 0 , 0);
  for(int i = 0; i < displayHeight-2; i ++){
    if(i < displayStord){
      mvwprintw(displayWin, displayHeight-i-2, 1, "%s", displayStor[i]);
    }else{
      break;
    }
  }
  wrefresh(displayWin);
  updateInput();
}

void initInterface(){
  initscr();
  cbreak();			/* Line buffering disabled, Pass on everty thing to me 		*/
  noecho();
  keypad(stdscr, TRUE);
  refresh();
  displayHeight = LINES - inputHeight;
  width = COLS;
  inputStord = 0;
  inputStor[inputStord] = '\0';
  displayStord = 0;
  QUIT_KEY = KEY_F(1);
  input();
  display();
  updateInput();
  updateDisplay();
}

void destroyInterface(){
  pthread_cancel(updateThread);
  delwin(inputWin);
  delwin(displayWin);
  endwin();
}

char *getInput(){
  while(1){
    int c = getch();
    switch(c){
      case '\n':
        inputStor[inputStord] = '\0';
        char *str = malloc(INPUSTOR+1);
        strcpy(str, inputStor);
        //displayNew(inputStor);
        inputStord = 0;
        inputStor[inputStord] = '\0';
        //updateDisplay();
        updateInput();
        return str;
      case 127:
      case KEY_BACKSPACE:
        if(inputStord > 0){
          inputStord--;
          inputStor[inputStord] = '\0';
          updateInput();
          continue;
        }
        break;
    }
    if(inputStord < INPUSTOR-2 && c >= ' ' && c <= '~'){
      inputStor[inputStord++] = (char)c;
      inputStor[inputStord] = '\0';
    }
    updateInput();
  }
}

void displayNew(char n[]){
  for(int i =displayStord; i > 0; i--){
    if(i < DISPSTOR){
      strcpy(displayStor[i], displayStor[i-1]);
    }
  }
  int w = 0;
  while(n[w] != '\0' && n[w] !='\n'){
    displayStor[0][w] = n[w];
    w++;
  }
  displayStor[0][w] = '\0';
  if(displayStord < DISPSTOR){
    displayStord++;
  }
  updateDisplay();
}
void displayPrint(char u[], char n[]){
  char msg[128];
  snprintf(msg, 126, "%s %s", u, n);
  displayNew(msg);
}
WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}
