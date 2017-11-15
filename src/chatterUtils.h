#ifndef CHATTERUTILS_H
#define CHATTERUTILS_H
void sleep_ms(int milliseconds);
char **getCommand(char *inp);
int prefix(const char *pre, const char *str);
char* replaceChar(char* str, char find, char replace);
#endif
