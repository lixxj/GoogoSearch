#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "readData.h"
#include "StrDict.h"
#include "strdup.h"

#define MAX_LINE 1000

typedef struct _match {
    char *page;
    double tfidf;
} match;


static void scanPastLine(FILE *fin, char *line) {
    char buf[MAX_LINE] = {0};
    while (buf == fgets(buf, MAX_LINE, fin) && strcmp(buf, line) != 0){}
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

double calcTfIdf(char *term, char *document, int nDoc, int nContaining) {
    FILE *doc = openExt(document, ".txt");
    scanPastLine(doc, "#start Section-2\n");
    
    char wbuf[MAX_LINE] = {0};
    int tCount = 0;
    int wCount = 0;
    
    while (fscanf(doc, "%s ", wbuf) == 1 && strcmp(wbuf, "#end") != 0) {
        lower(wbuf);
        stripSpecial(wbuf);
        if (strcmp(wbuf, term) == 0) tCount++;
        wCount++;
    }
    
/*    printf("term: %s doc: %s tCount: %d wCount: %d nDoc: %d nCont: %d\n", */
/*            term, document, tCount, wCount, nDoc, nContaining);*/
    
    double tf = (double)tCount/(double)wCount;
    double idf = log10((double)nDoc/(double)nContaining);
    
    fclose(doc);
    
    return tf * idf;
}

int countWords(char *txt) {
    int count = 0;
    int onWord = 0;
    int curr = 0;
    int len = strlen(txt);
    
    while (curr < len) {
    
        if (onWord == 0) {
            if (txt[curr] != ' ') onWord = 1;
        }else if (txt[curr] == ' ') {
            count++;
            onWord = 0;
        }
        curr++;
    }
    
    return count;
}

int matchSort(const void *m1, const void *m2) {
    return ( ((match*)m1)->tfidf < ((match*)m2)->tfidf );
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./searchTfIdf {term}\n");
        exit(1);
    }
    SList pages = getPages("collection.txt");
    int nDoc = nStr(pages);
    
    freeSList(pages);
        
    FILE *iindex = fopen("invertedIndex.txt", "r");
    if (iindex == NULL) {
        fprintf(stderr, "Cannot open invertedIndex.txt\n");
        abort();
    }
    
    match *tfidfs = calloc(nDoc, sizeof(match));    //tfidf of pages: indexed using below dictionary
    StrDict pageMatches = newStrDict();
    int currIndex = 0;
    
    char buf[MAX_LINE] = {0};
    
    while (fscanf(iindex, "%s", buf) == 1) {   //for each line (take first word)
    
        int i = 1;
        while (i < argc) {  //for each possible search term
            if (strcmp(buf, argv[i]) == 0) {        //if term matches current entry
                
                if( buf != fgets(buf, MAX_LINE, iindex)) abort();       //grab rest of line
                buf[strlen(buf)-1] = '\0';          //strip newline
                int nOccur = countWords(buf);
                char *url = strtok(buf, " ");       //begin separating page names
                
                while (url != NULL) {               //for each page in the entry
                    
                    if (lookup(pageMatches, url) == -1) {  //if we haven't come across the page yet
                        
                        pageMatches = addIndex(pageMatches, currIndex, url);
                        tfidfs[currIndex].page = strdup(url);
                        currIndex++;
                    }
                    
                    tfidfs[lookup(pageMatches, url)].tfidf += calcTfIdf(argv[i], url, nDoc, nOccur);
                    url = strtok(NULL, " ");
                }
                break;  //skip checking other search terms
            }
            i++;
        }
        if (fscanf(iindex, "\n") != 0) abort();   //go to next line
    }
    
    freeStrDict(pageMatches);
    qsort(tfidfs, currIndex, sizeof(match), matchSort);
    
    if (currIndex > 30) currIndex = 30;
    
    for (int i = 0; i < currIndex; i++)
        printf("%s %.9f\n", tfidfs[i].page, tfidfs[i].tfidf);
    
    for (int i = 0; i < nDoc; i++)
        free(tfidfs[i].page);
        
    free(tfidfs);
    fclose(iindex);
    
    return 0;
}
