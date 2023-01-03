#include "ObtainInput.h"

void obtainInput(ListNodePtr newPtr);
char* extendBuffer(char* buffer, size_t* sizePtr);
int isDigit(char c);
int isPeriod(char c);

void obtainInput(ListNodePtr newPtr)
{
	char c, * buffer = NULL;
	size_t size = 0;
	int isString = 0;
	int _isPeriod = 0;

	while ((c = getchar()) != '\n') {
		buffer = extendBuffer(buffer, &size);
		buffer[size - 1] = c;
	}
	buffer = extendBuffer(buffer, &size);
	buffer[size - 1] = '\0';

	for (int i = 0; i < size - 1; i++) {
		if (isPeriod(buffer[i]))
			_isPeriod = 1;
		else if (!isDigit(buffer[i])) {
			isString = 1;
			break;
		}
	}

	if (isString) {
		newPtr->type = 2;
		newPtr->dataPtr = buffer;
	}
	else if (_isPeriod) {
		double* dataPtr = malloc(sizeof(double));

		if (dataPtr)
			*dataPtr = atof(buffer);
		newPtr->type = 1;
		newPtr->dataPtr = dataPtr;
		free(buffer);
	}
	else {
		int* dataPtr = malloc(sizeof(int));

		if (dataPtr)
			*dataPtr = atoi(buffer);
		newPtr->type = 0;
		newPtr->dataPtr = dataPtr;
		free(buffer);
	}
}

char* extendBuffer(char* buffer, size_t* sizePtr)
{
	char* newBuffer = malloc(*sizePtr + 1);

	if (newBuffer)
		memcpy(newBuffer, buffer, *sizePtr);
	if (buffer)
		free(buffer);
	(*sizePtr)++;
	return newBuffer;
}

int isDigit(char c)
{
	return c >= '0' && c <= '9';
}

int isPeriod(char c)
{
	return c == '.';
}