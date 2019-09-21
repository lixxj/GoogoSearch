#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readData.h"
#include "SList.h"
#include "graph.h"

SList getPages(char *collecName) {
    SList urls = newSList();
    FILE *fin = fopen(collecName,"r");
    
    if (fin == NULL) {
        printf("File %s not found\n", collecName);
        abort();
    }
    
    char nameBuf[MAX_FILENAME] = {'\0'};
    
    while (fscanf(fin, " %255s", nameBuf) == 1) {
        
        urls = addStr(urls, nameBuf);
    }
    
    fclose(fin);
    
    return urls;
}

SList getLinks(char *pageName) {
    char *fileName = calloc(strlen(pageName)+strlen(".txt")+1, sizeof(char));
    strcat(fileName, pageName);
    strcat(fileName, ".txt");
    
    FILE *fin = fopen(fileName, "r");
    
    if (fin == NULL) {
        printf("File %s not found\n", fileName);
        abort();
    }
    
    SList links = newSList();
    char nameBuf[MAX_FILENAME] = {0};
    
    int success = 0;
    success = fscanf(fin, "#start Section-1 %255s ", nameBuf);     //%255 prevents buffer overflow
    
    while (strcmp(nameBuf, "#end") != 0 && success) {
        
        links = addStr(links, nameBuf);
        success = fscanf(fin, "%255s ", nameBuf);
    }
    
    fclose(fin);
    free(fileName);
    
    return links;
}

PageGraph buildGraph(SList pages) {
    int nUrls = nStr(pages);
    PageGraph new = newPageGraph(nUrls);
    
    int p = 0;
    
    while (p < nUrls) {
        
        char *curr = NULL;
        pages = popFirstStr(pages, &curr);
        SList outLinks = getLinks(curr);
        
        int i = 0;
        int nLinks = nStr(outLinks);
        
        while (i < nLinks) {
            
            char *link = NULL;
            outLinks = popFirstStr(outLinks, &link);
            addEdge(new, curr, link);
            free(link);
            i++;
        }
        free(curr);
        
        p++;
    }
    
    freeSList(pages);
    
    return new;
}
