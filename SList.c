#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SList.h"
#include "strdup.h"
//BST representation

#define isLess(a, b) (strcmp(a, b) < 0)
#define isGreater(a, b) (strcmp(a, b) > 0)
#define isEqual(a, b) (strcmp(a, b) == 0)

typedef struct _slist {

    char *str;
    struct _slist *left;
    struct _slist *right;
} sListRep;

SList newSList() {
    return NULL;
}
    
void freeSList(SList s) {
    
    if (s == NULL) return;
    
    if (s->left != NULL)
        freeSList(s->left);
    if (s->right != NULL)
        freeSList(s->right);
    
    free(s->str);
    free(s);
}

SList addStr(SList s, char *str) {

    if (s == NULL) {
        
        SList new = malloc(sizeof(sListRep));
        if (new == NULL) {
            fprintf(stderr, "Failed malloc");
            abort();
        }
        new->left = NULL;
        new->right = NULL;
        new->str = strdup(str);
        
        return new;
        
    } else if (isLess(str, s->str)) {
    
        s->left = addStr(s->left,str);
    }else if (isGreater(str, s->str)) {
    
        s->right = addStr(s->right, str);
    }
    return s;    // avoid duplicates
}
    
int isStr(SList s, char *str) {
    
    SList curr = s;
    int cmpResult = strcmp(str, curr->str);
    
    while (cmpResult != 0) {
    
        if (cmpResult < 0)
            curr = curr->left;
        else
            curr = curr->right;
        
        if (curr == NULL)
            return 0;
        else
            cmpResult = strcmp(str, curr->str);
    }
    
    return 1;
}
        
SList popFirstStr(SList s, char** str) {
    
    if (s == NULL){
        *str = NULL;
        return NULL;
    }
    if (s->left == NULL) {
        
        *str = s->str;
        SList right = s->right;
        free(s);
        return right;
    }else
        s->left = popFirstStr(s->left, str);
        
    return s;
}

int nStr(SList s) {
    
    if (s == NULL)
        return 0;
    return 1 + nStr(s->left) + nStr(s->right);
}

void dumpList(SList s, FILE *out, char *sep) {
    
    if (s == NULL) return;
    
    dumpList(s->left, out, sep);
    fprintf(out, "%s%s", s->str, sep);
    dumpList(s->right, out, sep);
}


