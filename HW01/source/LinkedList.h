#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

struct listNode {
	int type;
	void* dataPtr;
	struct listNode* nextPtr;
};

typedef struct listNode ListNode;
typedef ListNode* ListNodePtr;

void insert(ListNodePtr*);
ListNodePtr delete(ListNodePtr*);
int isEmpty(ListNodePtr);
void printList(ListNodePtr);

int compare(ListNodePtr, ListNodePtr);

#endif // !__LINKEDLIST_H__
