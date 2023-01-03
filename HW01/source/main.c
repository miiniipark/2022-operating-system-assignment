#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"

void instructions();

void instructions()
{
	printf("Enter your choice:\n"
		"	1 to insert an element into the list.\n"
		"	2 to delete an element from the list.\n"
		"	3 to end.\n");
}

int main()
{
	ListNodePtr startPtr = NULL;
	int choice, dump;

	instructions();
	printf("? ");
	dump = scanf("%d", &choice);
	dump = getchar();

	while (choice != 3) {

		switch (choice) {
		case 1:
			printf("Enter integer, float, or string: ");
			insert(&startPtr);
			printList(startPtr);
			break;
		case 2:
			if (!isEmpty(startPtr)) {
				ListNodePtr tempPtr;
				printf("Enter integer, float, or string to be deleted: ");

				if (tempPtr = delete(&startPtr)) {
					if (tempPtr->type == 0)
						printf("%d deleted.\n", *((int*)(tempPtr->dataPtr)));
					else if (tempPtr->type == 1)
						printf("%f deleted.\n", *((double*)(tempPtr->dataPtr)));
					else
						printf("%s deleted.\n", (char*)(tempPtr->dataPtr));
					printList(startPtr);
				}
				else
					printf("Not found.\n\n");
			}
			else
				printf("List is empty.\n\n");

			break;
		default:
			printf("Invalid choice.\n\n");
			instructions();
			break;
		}

		printf("? ");
		dump = scanf("%d", &choice);
		dump = getchar();
	}

	printf("End of run.\n");
	return 0;
}