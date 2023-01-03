#ifndef __OBTAININPUT_H__
#define __OBTAININPUT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedList.h"

void obtainInput(ListNodePtr newPtr);
char* extendBuffer(char* buffer, size_t* sizePtr);
int isDigit(char c);
int isPeriod(char c);

#endif // !__OBTAININPUT_H__
