#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StrDict.h"
#include "strdup.h"

#define isLess(a, b) (strcmp(a, b) < 0)
#define isGreater(a, b) (strcmp(a, b) > 0)

typedef struct _sdict {

    char *str;
    int id;
    struct _sdict *left;
    struct _sdict *right;
} strDictRep;

StrDict newStrDict(){
    return NULL;
}

//void delStr(StrDict d, char *str);

void freeStrDict(StrDict s) {
    
    if (s == NULL) return;
    
    if (s->left != NULL)
        freeStrDict(s->left);
    if (s->right != NULL)
        freeStrDict(s->right);
    
    free(s->str);
    free(s);
}

StrDict addIndex(StrDict s, int index, char *str) {

    if (s == NULL) {
        
        StrDict new = malloc(sizeof(strDictRep));
        if (new == NULL) {
            fprintf(stderr, "Failed malloc");
            abort();
        }
        new->left = NULL;
        new->right = NULL;
        new->id = index;
        new->str = strdup(str);
        
        return new;
        
    } else if (isLess(str, s->str)) {
    
        s->left = addIndex(s->left, index, str);
    }else if (isGreater(str, s->str)) {
    
        s->right = addIndex(s->right, index, str);
    }
    return s;    // avoid duplicates
}
    
int lookup(StrDict s, char *str) {
    
    if (s == NULL) return -1;
    StrDict curr = s;
    int cmpResult = strcmp(str, curr->str);
    
    while (cmpResult != 0) {
    
        if (cmpResult < 0)
            curr = curr->left;
        else
            curr = curr->right;
        
        if (curr == NULL)
            return -1;
        else
            cmpResult = strcmp(str, curr->str);
    }
    
    return curr->id;
}

int nEntries(StrDict d) {

    if (d == NULL) return 0;
    return 1 + nEntries(d->left) + nEntries(d->right);
}
