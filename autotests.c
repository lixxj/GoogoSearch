#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "SList.h"
#include "StrDict.h"
#include "IntList.h"
#include "graph.h"
#include "readData.h"

void testIntList() {
    //note that only the below functions urgently need testing
    
    IntList l = newIntList();
    IntListInsert(l, 1);
    IntListInsert(l, 2);
    IntListInsert(l, 10);
    
    assert(getIthVal(l, 0) == 1);
    assert(getIthVal(l, 1) == 2);
    assert(getIthVal(l, 2) == 10);
    
    assert(IntListContains(l, 1));
    assert(IntListContains(l, 2));
    assert(IntListContains(l, 10));
    
    freeIntList(l);
}

void testSList() {

    SList s = newSList();
    s = addStr(s, "Number 1");
    s = addStr(s, "AAAA");
    s = addStr(s, "Number 2");
    s = addStr(s, "Test 1");
    s = addStr(s, "Test 1");
    
    assert(isStr(s, "Number 1"));
    assert(isStr(s, "Number 2"));
    assert(isStr(s, "Test 1"));
    assert(!isStr(s, "Number 10"));
    assert(nStr(s) == 4);
    
    dumpList(s, stdout, " ");
    
    char *p = NULL;
    s = popFirstStr(s, &p);
    assert(strcmp("AAAA", p) == 0);
    free(p);
    
    s = popFirstStr(s, &p);
    assert(strcmp("Number 1", p) == 0);
    free(p);
    
    freeSList(s);
}

void testStrDict() {
    
    StrDict d = newStrDict();
    
    d = addIndex(d, 5, "Node 1");
    d = addIndex(d, 2, "Node 2");
    d = addIndex(d, 10, "Node 3");
    
    assert(lookup(d, "Node 1") == 5);
    assert(lookup(d, "Node 2") == 2);
    assert(lookup(d, "Node 3") == 10);
    assert(lookup(d, "not in") == -1);
    
    freeStrDict(d);
}

void graphTests() {
    
    PageGraph g = newPageGraph(5);
    
    assert(addEdge(g, "Node 1", "Node 2"));
    assert(addEdge(g, "Node 1", "Node 3"));
    assert(addEdge(g, "Node 3", "Node 4"));
    assert(addEdge(g, "Node 4", "Node 5"));
    
    showPageGraph(g);
    
    assert(isEdge(g, "Node 1", "Node 2"));
    assert(isEdge(g, "Node 1", "Node 3"));
    assert(isEdge(g, "Node 3", "Node 4"));
    assert(isEdge(g, "Node 4", "Node 5"));
    
    assert(nVertices(g) == 5);
    
    freePageGraph(g);
}

void testReadData() {
    
    SList pages = getPages("collection.txt");
    dumpList(pages, stdout, " ");
    freeSList(pages);
}

void testExtract() {
    
    SList links = getLinks("url11");
    dumpList(links, stdout, " ");
    freeSList(links);
}

void testBuildGraph() {

    SList pages = getPages("collection.txt");
    PageGraph web = buildGraph(pages);
    
    showPageGraph(web);
    
    freePageGraph(web);
}

int main(int argc, char *argv[]) {

    testSList();
    testStrDict();
    testIntList();
    graphTests();
    testReadData();
    testExtract();
    testBuildGraph();
        
    return 0;
}
