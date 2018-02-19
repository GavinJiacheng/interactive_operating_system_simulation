 #include "list.h"


 LIST *ListCreate()
 {
     int i;
     LIST *ret;
     if( listArray[0].nextFree == NULL || listArray[0].nextFree == NULL)
     {
        for (i = 0; i < MAXLIST-1; i++)
        {
            listArray[i].nextFree = &listArray[i + 1];
        }
        listArray[MAXLIST-1].nextFree = NULL;
        nextFreeList = &listArray[0];

        for (i = 0; i < MAXNODE-1; i++)
        {
            nodeArray[i].next = &nodeArray[i+1];
        }
        nodeArray[MAXNODE-1].next = NULL;
        nextFreeNode = &nodeArray[0];
     }


    if(nextFreeList == NULL)
        return NULL;
    else
    {
        ret = nextFreeList;
        ret->head = NULL;
        ret->last = NULL;
        ret->len = 0;
        nextFreeList = ret->nextFree;
        return ret;
    }
 }



 NODE *NodeCreate(void *item, NODE *NEXTN, NODE *prev)
{
    NODE *ret;
     if (nextFreeNode == NULL)
         return NULL;
     ret = nextFreeNode;
     nextFreeNode = ret->next;
     ret->item = item;
     ret->next = NEXTN;
     ret->prev = prev;

     return ret;

 }


int ListAdd(LIST *list, void *item)
{
    NODE *newNode;
    if(list->head == NULL)
    {
        newNode = NodeCreate(item, NULL, NULL);
        if(newNode == NULL)
            return -1;
        list->head = newNode;
        list->last = newNode;
        list->curr = newNode;
        list->len = list->len + 1;
        return 0;
    }
    newNode = NodeCreate(item, list->curr->next, list->curr);
	if( newNode == NULL)
	{
		return -1;
	}
	else
	{
        if(list->curr->next == NULL)
		{
            list->curr->next = newNode;
			list->curr = newNode;
            list->last = newNode;
		}
        else
        {

			list->curr->next->prev = newNode;
			list->curr->next= newNode;
            list->curr = newNode;
        }

		list->len = list->len + 1;
	}
	return 0;
 }


int ListInsert(LIST *list, void *item)
{
    NODE *newNode;

    if(list->head == NULL)
    {
        newNode = NodeCreate(item, NULL, NULL);
        if(newNode == NULL)
            return -1;
        list->head = newNode;
        list->last = newNode;
        list->curr = newNode;
        list->len = list->len + 1;
        return 0;
    }

	newNode = NodeCreate(item, list->curr, list->curr->prev);
	if( newNode == NULL)
	{
		return -1;
	}
	else
	{
		if(list->curr->prev == NULL)
		{
			list->curr->prev = newNode;
			list->head = newNode;
		}
        else
        {
			list->curr->prev->next = newNode;
			list->curr->prev = newNode;
		}
        list->curr = newNode;
		list->len = list->len + 1;
	}
	return 0;
 }


 int ListAppend(LIST *list, void *item)
{
    NODE *newNode;
    if(list->head == NULL)
    {
        newNode = NodeCreate(item, NULL, NULL);
        if(newNode == NULL)
            return -1;
        list->head = newNode;
        list->last = newNode;
        list->curr = newNode;
        list->len = list->len + 1;
        return 0;
    }
	newNode = NodeCreate(item, NULL, list->last);

	if( newNode == NULL)
	{
		return -1;
	}
    else
    {
		list->last->next = newNode;
		list->last = newNode;
	}
    list->curr = newNode;
	list->len = list->len + 1;

	return 0;
 }


int ListPrepend(LIST *list, void *item)
{
    NODE *newNode;
    if(list->head == NULL)
    {
        newNode = NodeCreate(item, NULL, NULL);
        if(newNode == NULL)
            return -1;
        list->head = newNode;
        list->last = newNode;
        list->curr = newNode;
        list->len = list->len + 1;
        return 0;
    }

    newNode = NodeCreate(item, list->head, NULL);
	if( newNode == NULL)
	{
		return -1;
	}
    else
    {
		list->head->prev = newNode;
		list->head = newNode;
	}
    list->curr = newNode;
	list->len = list->len + 1;

	return 0;
 }



int ListCount ( LIST *list)
{
	return list->len;
}

void *ListFirst (LIST *list)
{
    if (list->head == NULL)
	{
        return NULL;
    }
    else
    {
		list->curr = list->head;
	}

    return list->curr->item;
}

void *ListLast (LIST *list)
{
    if (list->last == NULL)
        return NULL;

    list->curr = list->last;

    return list->last->item;
}

void *ListNext (LIST *list)
{
    if (list->curr == NULL)
        return NULL;

	if (list->curr->next == NULL)
        return NULL;
    else
		list->curr = list->curr->next;

    return list->curr->item;
}

void *ListPrev (LIST *list)
{
    if(list->curr == NULL)
        return NULL;

    if (list->curr->prev == NULL)
        return NULL;
    else
        list->curr = list->curr->prev;

    return list->curr->item;
}

void *ListCurr(LIST *list)
{
    if(list->curr == NULL)
        return NULL;
    else
        return list->curr->item;
}


 void *ListRemove(LIST *list)
{
   void *currItem;
   NODE *nodeToBeDeleted;
   NODE *ret;

   currItem = list->curr->item;
   nodeToBeDeleted = list->curr;
   ret = list->curr->next;
   list->curr->item = NULL;
   if(list->len == 1)
   {
       list->head = NULL;
       list->last = NULL;
   }
   else if(list->head == list->curr)
   {
       list->head = list->head->next;
   }
   else if(list->last == list->curr)
   {
        list->last = list->last->prev;
   }
   else
   {
       list->curr->prev->next = list->curr->next;
       list->curr->next->prev = list->curr->prev;
   }

   nodeToBeDeleted->prev = NULL;
   nodeToBeDeleted->next = nextFreeNode;
   nextFreeNode = nodeToBeDeleted;

   list->curr = ret;
   list->len--;
   return currItem;
 }


 void *ListSearch (LIST *list, int (*comparator)(void*, void*), void *comparisonArg)
 {
     if ( list->head == NULL || list->curr == NULL || list->curr->item == NULL)
         return NULL;

     if((*comparator)(list->curr->item,comparisonArg) == 1)
         return list->curr->item;

     while(list->curr->next != NULL)
     {
         list->curr = list->curr->next;
         if((*comparator)(list->curr->item,comparisonArg) == 1)
             return list->curr->item;
     }

     return NULL;
 }
