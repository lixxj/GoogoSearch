////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2019, XJ, All rights reserved.
//
// This is an unpublished, proprietary work of XJ Li and is fully
// protected under copyright and trade secret law. You may not view, use,
// disclose, or distribute this file or any information contained herein except
// persuant to a valid written license from XJ Li.
//
// The entire notice above must be reproduced on all authorized copies.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "SList.h"
#include "readData.h"
#include "strdup.h"

#define MAX_LINE 1000

typedef struct _iinode{
    char *word;
    SList occurs;
    struct _iinode *left;
    struct _iinode *right;
} iinode;

typedef struct _iindex{
    iinode *tree;
} iindex;

typedef struct _iindex *IIndex;

IIndex newIIndex() {
    IIndex new = calloc(1, sizeof(iindex));
    if (new == NULL) {
        printf("Failed malloc\n");
        abort();
    }
    
    return new;
}

static void freeTree(iinode *n) {
    // base situation
    if (n == NULL) return;
    
    if (n->left != NULL)
        freeTree(n->left);
    if (n->right != NULL)
        freeTree(n->right);
    
    free(n->word);
    freeSList(n->occurs);
    free(n);
}

void freeIIndex(IIndex toFree) {
    freeTree(toFree->tree);
    free(toFree);
}

static iinode* addpair(iinode *i, char *word, char *occurs) {
    if (i == NULL) {    //allocate new word and add the occurrence
        iinode* new = malloc(sizeof(iinode));
        if (new == NULL) abort();
        new->word = strdup(word);
        new->occurs = newSList();
        new->occurs = addStr(new->occurs, occurs);
        new->left = NULL;
        new->right = NULL;
        return new;
    }

    if (strcmp(word, i->word) < 0)
        i->left = addpair(i->left, word, occurs);
    else if (strcmp(word, i->word) > 0)
        i->right = addpair(i->right, word, occurs);
    else {
        i->occurs = addStr(i->occurs, occurs);  //located word
        return i;
    }
    return i;
}

void addPair(IIndex i, char *word, char* occurs) {
    // base situation
    if (i == NULL || word == NULL || occurs == NULL) return;
    
    i->tree = addpair(i->tree, word, occurs);
}

static void inOrder(iinode *i, FILE *fout) {
    // base situation
    if (i == NULL) return;
    
    inOrder(i->left, fout);
    fprintf(fout, "%s ", i->word);
    dumpList(i->occurs, fout, " ");
    fprintf(fout, "\n");
    inOrder(i->right, fout);
    return;
}

void dumpIIndex(IIndex i, FILE *fout) {
    inOrder(i->tree, fout);
}

static FILE *openExt(char *fileName, char *ext) {
    //open a file with file extension appended to the name
    char *buf = calloc(sizeof(char), strlen(fileName)+strlen(ext)+1);
    if (buf == NULL) abort();
    strcat(buf, fileName);
    strcat(buf, ext);
    
    FILE *fout = fopen(buf, "r");
    if (fout == NULL) {
        fprintf(stderr, "Could not open file %s\n", buf);
        abort();
    }
    free(buf);
    return fout;
}

void lower(char *w) {
    int len = strlen(w);
    for (int i = 0; i < len; i++)
        w[i] = tolower(w[i]);
}

void stripSpecial(char *w) {
    for (int i = 0; w[i] != '\0' ; i++) {
        if (w[i] == '.' || w[i] == ',' || w[i] == ';' || w[i] == '?') {
            w[i] = '\0';
            break;
        }
    }
}

void scanPastLine(FILE *fin, char *line) {
    char buf[MAX_LINE] = {0};
    while (buf == fgets(buf, MAX_LINE, fin) && strcmp(buf, line) != 0){}
}

int main (int argc, char *argv[]) {
    SList pages = getPages("collection.txt");
    int nPages = nStr(pages);
    IIndex i = newIIndex();
    int p = 0;
    
    while (p < nPages) {
        char *pageName = NULL;
        pages = popFirstStr(pages, &pageName);
        FILE *page = openExt(pageName, ".txt");
        
        scanPastLine(page, "#start Section-2\n");
        char wordBuf[MAX_LINE] = {0};
        
        while (fscanf(page, " %s ", wordBuf) == 1 && strcmp(wordBuf, "#end") != 0) {
            
            lower(wordBuf);
            stripSpecial(wordBuf);
        
            //printf("Found %s on page %s.\n", wordBuf, pageName);
            addPair(i, wordBuf, pageName);
        }
                
        fclose(page);
        free(pageName);
        p++;
    }
    
    FILE *fout = fopen("invertedIndex.txt", "w");
    if (fout == NULL) {
        fprintf(stderr, "Could not open invertedIndex.txt for writing\n");
        abort();
    }
    
    dumpIIndex(i, fout);
    fclose(fout);
    freeIIndex(i);
    
    return 0;
}
