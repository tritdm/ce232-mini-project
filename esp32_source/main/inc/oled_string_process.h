#ifndef __OLED_STRING_PROCESS__
#define __OLED_STRING_PROCESS__

#include <string.h>
#include <math.h>

void reverse(char* str, int len);
int intToStr(int x, char str[], int d);
void ftoa(float n, char* res, int afterpoint);
char* oled_string(const char *humidity, const char *temperature);

#endif