#ifndef SLIST_H
#define SLIST_H
//list ADT for strings
//stores strings in order

typedef struct _slist *SList;

SList newSList();
void freeSList(SList s);

SList addStr(SList s, char *str);
SList popFirstStr(SList s, char** str);     //returned in str: can free with malloc
int isStr(SList s, char *str);
int nStr(SList s);  //number of members
void dumpList(SList s, FILE *out, char *sep);   //inorder traversal. sep = separator string

#endif
