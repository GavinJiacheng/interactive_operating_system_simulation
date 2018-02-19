
 #ifndef LIST_H
 #define LIST_H

 #include <stdio.h>
 #include <stdlib.h>

#define MAXLIST   2048
#define MAXNODE   2048

typedef struct LIST{
    struct NODE  *head;
    struct NODE  *last;
    struct NODE  *curr;
    int     len;
    struct LIST *nextFree;
}LIST;

typedef struct NODE{
    void *item;
    struct NODE *next;
    struct NODE *prev;
}NODE;


LIST listArray[MAXLIST];
NODE nodeArray[MAXNODE];


LIST *nextFreeList;
NODE *nextFreeNode;

LIST *ListCreate();
NODE *NodeCreate(void *item, NODE *NEXTN, NODE *prev);
int ListAdd(LIST *list, void *item);
int ListInsert(LIST *list, void *item);
int ListAppend(LIST *list, void *item);
int ListPrepend(LIST *list, void *item);
int ListCount ( LIST *list);
void *ListFirst (LIST *list);
void *ListLast (LIST *list);
void *ListNext (LIST *list);
void *ListPrev (LIST *list);
void *ListCurr(LIST *list);
void *ListRemove(LIST *list);
void *ListSearch (LIST *list, int (*comparator)(void*, void*), void *comparisonArg);

#endif
