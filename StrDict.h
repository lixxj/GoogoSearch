#ifndef STRDICT_H
#define STRDICT_H

typedef struct _sdict *StrDict;

StrDict newStrDict();
void freeStrDict(StrDict d);

StrDict addIndex(StrDict d, int index, char *str);
int lookup(StrDict d, char *str);   //return -1 if not found, otherwise positive integer
void delIndex(StrDict d, char *str);
int nEntries(StrDict d);

#endif
