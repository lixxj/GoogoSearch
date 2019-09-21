// IntList.c - Lists of integers
// Written by John Shepherd, July 2008

#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "IntList.h"

// data structures representing IntList

typedef struct IntListNode {
	int  data;  // value of this list item
	struct IntListNode *next;
	            // pointer to node containing next element
} intListNode;

struct IntListRep {
	int  size;  // number of elements in list
	struct IntListNode *first;
	            // node containing first value
	struct IntListNode *last;
	            // node containing last value
	            
	struct IntListNode *travNode;
	int travIndex;
} intListRep;

// create a new empty IntList
IntList newIntList()
{
	struct IntListRep *L;

	L = malloc(sizeof (struct IntListRep));
	assert (L != NULL);
	L->size = 0;
	L->first = NULL;
	L->last = NULL;
	
	L->travIndex = 0;
	L->travNode = NULL;
	return L;
}

// free up all space associated with list
void freeIntList(IntList L)
{
	intListNode *curr = L->first;
	
	while (curr != NULL) {
	    intListNode *next = curr->next;
	    free(curr);
	    curr = next;
	}
	
	free(L);
}

int getIthVal(IntList L, int index) {
    
    int val = OUT_OF_BOUNDS;
    
    if (index >= L->size)   return val;
    
    if (index == L->travIndex) {
    //if next in traversal
        val = L->travNode->data;
        L->travNode = L->travNode->next;
        L->travIndex++;
    } else {
    //go from front of list, reset traversal
        int i = 0;
        intListNode *curr = L->first;
        
        while (i < index) {
            curr = curr->next;
            i++;
        }
        val = curr->data;
        L->travIndex = i+1;
        L->travNode = curr->next;
    }
    return val;
}

int IntListContains(IntList L, int val) {

    intListNode *curr = L->first;
    
    while (curr != NULL && curr->data != val)
        curr = curr->next;
    
    if (curr == NULL)
        return 0;
    
    return 1;
}

// display list as one integer per line on stdout
void showIntList(IntList L)
{
	IntListPrint(stdout, L);
}

// create an IntList by reading values from a file
// assume that the file is open for reading
IntList getIntList(FILE *inf)
{
	IntList L;
	int v;

	L = newIntList();
	while (fscanf(inf,"%d",&v) != EOF)
		IntListInsert(L,v);
	return L;
}

// create a new IntListNode with value v
// (this function is local to this ADT)
static struct IntListNode *newIntListNode(int v)
{
	struct IntListNode *n;

	n = malloc(sizeof (struct IntListNode));
	assert(n != NULL);
	n->data = v;
	n->next = NULL;
	return n;
}

// apppend one integer to the end of a list
void IntListInsert(IntList L, int v)
{
	struct IntListNode *n;

	assert(L != NULL);
	n = newIntListNode(v);
	
	if (L->first == NULL) {
		L->first = L->last = n;
		L->travNode = n;
	}else {
		L->last->next = n;
		L->last = n;
	}
	L->size++;
}

// insert an integer into correct place in a sorted list
void IntListInsertInOrder(IntList L, int v)
{
	if (!IntListIsSorted(L) || L == NULL)
	    return;
    
    struct IntListNode *new = malloc(sizeof(struct IntListNode));
    
    struct IntListNode *n = L->first;
    
    //if empty list
    if (L->size == 0) {
        
        L->first = new;
        new->next = NULL;
    
    //insert at head
   } else if (n->data > v) {
            
        new->next = L->first;
        L->first = new;
        
    //if >1 element
    }else{
    
        while (n->next != NULL && n->next->data < v ) {
            
            n = n->next;
        }
        
        new->next = n->next;
        n->next = new;
    }
    
    new->data = v;
    
    if (new->next == NULL)
        L->last = new;
        
    L->size++;
    
}

// delete first occurrence of v from a list
// if v does not occur in List, no effect
void IntListDelete(IntList L, int v)
{
	struct IntListNode *curr, *prev;

	assert(L != NULL);

	// find where v occurs in list
	prev = NULL; curr = L->first;
	while (curr != NULL && curr->data != v) {
		prev = curr;
		curr = curr->next;
	}
	// not found; give up
	if (curr == NULL) return;
	// unlink curr
	if (prev == NULL)
		L->first = curr->next;
	else
		prev->next = curr->next;
	if (L->last == curr)
		L->last = prev;
	L->size--;
	// remove curr
	free(curr);
}

// return number of elements in a list
int IntListLength(IntList L)
{
	assert(L != NULL);
	return L->size;
}

// make a physical copy of a list
// new list looks identical to original list
IntList IntListCopy(IntList L)
{
	struct IntListRep *Lnew;
	struct IntListNode *curr;

	Lnew = newIntList();
	for (curr = L->first; curr != NULL; curr = curr->next)
		IntListInsert(Lnew, curr->data);
	return Lnew;
}

// make a sorted physical copy of a list
IntList IntListSortedCopy(IntList L)
{
	struct IntListRep *Lnew;
	struct IntListNode *curr;

	Lnew = newIntList();
	for (curr = L->first; curr != NULL; curr = curr->next)
		IntListInsertInOrder(Lnew, curr->data);
	return Lnew;
}

// check whether a list is sorted in ascending order
// returns 0 if list is not sorted, returns non-zero if it is
int IntListIsSorted(IntList L)
{
	struct IntListNode *curr;

	assert(L != NULL);
	// trivial cases, 0 or 1 items
	if (L->size < 2)
		return 1;
	// scan list, looking for out-of-order pair
	for (curr = L->first; curr->next != NULL; curr = curr->next) {
		if (curr->next->data < curr->data)
			return 0;
	}
	// nothing out-of-order, must be sorted
	return 1;
}

// check sanity of an IntList (for debugging)
int IntListOK(IntList L)
{
	struct IntListNode *p;
	int count;

	if (L == NULL)
		return 1;
	if (L->size == 0)
		return (L->first == NULL && L->last == NULL);

	// scan to (but not past) last node
	count = 1; // at least one node
	for (p = L->first; p->next != NULL; p = p->next)
		count++;

	return (count == L->size && p == L->last);
}

// display list as one integer per line to a file
// assume that the file is open for writing
void IntListPrint(FILE *outf, IntList L)
{
	struct IntListNode *curr;

	assert(L != NULL);
	for (curr = L->first; curr != NULL; curr = curr->next)
		printf("%d->", curr->data);
	printf("X");
}
