#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"
#include "ObtainInput.h"

void insert(ListNodePtr* sPtr)
{
	ListNodePtr newPtr, previousPtr, currentPtr;

	newPtr = malloc(sizeof(ListNode));

	if (newPtr != NULL) {
		obtainInput(newPtr);
		newPtr->nextPtr = NULL;

		previousPtr = NULL;
		currentPtr = *sPtr;

		while (compare(newPtr, currentPtr)) {
			previousPtr = currentPtr;
			currentPtr = currentPtr->nextPtr;
		}

		if (previousPtr == NULL) {
			newPtr->nextPtr = *sPtr;
			*sPtr = newPtr;
		}
		else {
			previousPtr->nextPtr = newPtr;
			newPtr->nextPtr = currentPtr;
		}
	}
	else
		printf("Not inserted. No memory available.\n");
}

ListNodePtr delete(ListNodePtr* sPtr)
{
	ListNodePtr previousPtr, currentPtr, tempPtr, inputPtr;

	inputPtr = malloc(sizeof(ListNode));
	if (inputPtr) {
		obtainInput(inputPtr);

		if (inputPtr->type == (*sPtr)->type) {
			if ((inputPtr->type == 0 && *((int*)(inputPtr->dataPtr)) == *((int*)((*sPtr)->dataPtr))) ||
				(inputPtr->type == 1 && *((double*)(inputPtr->dataPtr)) == *((double*)((*sPtr)->dataPtr))) ||
				(inputPtr->type == 2 && !strcmp((char*)(inputPtr->dataPtr), (char*)((*sPtr)->dataPtr)))) {

				tempPtr = *sPtr;
				*sPtr = (*sPtr)->nextPtr;
				free(tempPtr->dataPtr);
				free(tempPtr);
				return inputPtr;
			}
		}

		previousPtr = *sPtr;
		currentPtr = (*sPtr)->nextPtr;

		while (compare(inputPtr, currentPtr)) {
			previousPtr = currentPtr;
			currentPtr = currentPtr->nextPtr;
		}

		if (currentPtr != NULL) {
			if ((inputPtr->type == 0 && *((int*)(inputPtr->dataPtr)) == *((int*)(currentPtr->dataPtr))) ||
				(inputPtr->type == 1 && *((double*)(inputPtr->dataPtr)) == *((double*)(currentPtr->dataPtr))) ||
				(inputPtr->type == 2 && !strcmp((char*)(inputPtr->dataPtr), (char*)(currentPtr->dataPtr)))) {

				tempPtr = currentPtr;
				previousPtr->nextPtr = currentPtr->nextPtr;
				free(tempPtr->dataPtr);
				free(tempPtr);
				return inputPtr;
			}
		}
	}
	return NULL;
}

int isEmpty(ListNodePtr sPtr)
{
	return sPtr == NULL;
}

void printList(ListNodePtr currentPtr)
{
	if (currentPtr == NULL)
		printf("List is empty.\n\n");
	else {
		printf("The list is:\n");

		while (currentPtr != NULL) {
			if (currentPtr->type == 0)
				printf("%d --> ", *((int*)(currentPtr->dataPtr)));
			else if (currentPtr->type == 1)
				printf("%f --> ", *((double*)(currentPtr->dataPtr)));
			else
				printf("%s --> ", ((char*)(currentPtr->dataPtr)));
			currentPtr = currentPtr->nextPtr;
		}

		printf("NULL\n\n");
	}
}

int compare(ListNodePtr newNode, ListNodePtr oldNode)
{
	if (oldNode) {
		if (newNode->type == 2) {
			if (oldNode->type != 2)
				return 1;
			else if (((char*)(newNode->dataPtr))[0] > ((char*)(oldNode->dataPtr))[0])
				return 1;
		}
		else if (newNode->type == 0) {
			if (oldNode->type == 0 && *((int*)(newNode->dataPtr)) > *((int*)(oldNode->dataPtr)))
				return 1;
			else if (oldNode->type == 1 && *((int*)(newNode->dataPtr)) > *((double*)(oldNode->dataPtr)))
				return 1;
		}
		else {
			if (oldNode->type == 0 && *((double*)(newNode->dataPtr)) > *((int*)(oldNode->dataPtr)))
				return 1;
			else if (oldNode->type == 1 && *((double*)(newNode->dataPtr)) > *((double*)(oldNode->dataPtr)))
				return 1;
		}
	}
	return 0;
}