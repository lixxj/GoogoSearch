#ifndef READDATA_H
#define READDATA_H

#include "SList.h"
#include "graph.h"

#define MAX_FILENAME 256

SList getPages(char *collecName);	  //pass "collection.txt" to this
PageGraph buildGraph(SList pages);  //NOTE: destroys the list of pages
SList getLinks(char *pageName);     //get a list of all the outgoing links of a page

#endif
