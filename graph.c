#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "IntList.h"
#include "StrDict.h"
#include "graph.h"
#include "strdup.h"

static void msgAbort(char *msg) {
    printf("Error: %s\n", msg);
    abort();
}

// ADT for URLs
typedef struct _vertex {
    IntList outgoing;
    IntList incoming;
    char *name;
    double pagerank;
} vertex;

typedef struct _graphRep {
    vertex **vertices;  // array of pointers to vertex structs (URLs)
    int nV;
    int maxV;
    int nE;
    StrDict urls; // URL name to index in graph
} graphRep;

PageGraph newPageGraph(int nV) {

    PageGraph new = malloc(sizeof(graphRep));   //allocate main struct
    if (new == NULL)
        msgAbort("Failed malloc");
    
    new->maxV = nV;
    new->nV = 0;
    new->nE = 0;
    
    new->vertices = calloc(nV, sizeof(vertex*));    //allocate vertex list
    if (new->vertices == NULL)
        msgAbort("Failed malloc");
        
    new->urls = newStrDict();
    
    return new;
}

void freePageGraph(PageGraph toFree) {

    int freedV = 0;
    
    while (freedV < toFree->nV) {
        
        freeIntList(toFree->vertices[freedV]->incoming);
        freeIntList(toFree->vertices[freedV]->outgoing);
        free(toFree->vertices[freedV]->name);
        free(toFree->vertices[freedV]);
        freedV++;
    }
    free(toFree->vertices);
    
    freeStrDict(toFree->urls);
    free(toFree);
}

int nVertices(PageGraph g) {
    return g->nV;
}

// 
int nOutgoing(PageGraph g, int i) {
    return IntListLength(g->vertices[i]->outgoing);
}

// 
double getPR(PageGraph g, int i) {
    return g->vertices[i]->pagerank;
}

// 
char* getName(PageGraph g, int i) {
    return g->vertices[i]->name;
}

static int newIndex(PageGraph g, char *name) {

    int nI = g->nV;
    
    g->vertices[nI] = malloc(sizeof(vertex));
    if (g->vertices[nI] == NULL)
        msgAbort("Failed malloc");
        
    g->vertices[nI]->outgoing = newIntList();
    g->vertices[nI]->incoming = newIntList();
    
    if (g->vertices[nI]->outgoing == NULL || 
        g->vertices[nI]->incoming == NULL   )
            msgAbort("Failed list allocation");
    
    g->nV++;
    
    g->urls = addIndex(g->urls, nI, name);
    
    g->vertices[nI]->name = strdup(name);
    
    if (g->vertices[nI]->name == NULL)
        msgAbort("Failed malloc");
        
    return nI;
}

int addEdge(PageGraph g, char *u, char *v) {
    
    int added = 0;
        
    int uIndex = lookup(g->urls, u);
    int vIndex = lookup(g->urls, v);
    
    if (uIndex == -1) added++;
    if (vIndex == -1) added++;
    
    if (g->nV+added > g->maxV) return 0;   //do not overflow vertex array
    if (uIndex != -1 && uIndex == vIndex) return 0;         //do not allow self-links
    if (isEdge(g, u, v)) return 0;               //do not allow parallel edges
                
    if (uIndex == -1) {     //if u not a vertex
        uIndex = newIndex(g, u);
    }
    
    if (vIndex == -1) {
        vIndex = newIndex(g, v);
    }
    
    IntListInsert(g->vertices[uIndex]->outgoing, vIndex);
    IntListInsert(g->vertices[vIndex]->incoming, uIndex);
    return added;
}

int isEdge(PageGraph g, char *u, char *v) {
    
    int uIndex = lookup(g->urls, u);
    int vIndex = lookup(g->urls, v);
    
    if (uIndex == -1 || vIndex == -1)
        return 0;
    
    return IntListContains( g->vertices[uIndex]->outgoing, vIndex );
}

void showPageGraph(PageGraph g) {

    for (int i = 0; i < g->nV; i++) {
        
        IntListPrint(stdout, g->vertices[i]->incoming);
        printf(" into %s, which links to ", g->vertices[i]->name);
        IntListPrint(stdout, g->vertices[i]->outgoing);
        printf("\n");
    }
}

// template function to calculate Win
// the weight of link(v->u) calculated based on
// the number of inlinks of page u and the number of inlinks
// of all reference pages of page v
static double inLinkW(PageGraph p, int v, int u) {
    // number of inlinks of u
    double u_in = IntListLength(p->vertices[u]->incoming);
    if (u_in == 0) return 0;   //skip unecessary work
    // iterate through pages that v points to and calculate the sum inlinks they have
    double sum_in = 0;
    double outl = IntListLength(p->vertices[v]->outgoing);   //number of links outgoing from v
    
    for (int i = 0; i < outl; i++) {   //for each outlink of v
    
        int curr = getIthVal(p->vertices[v]->outgoing, i);      //id of v outlink
        sum_in += IntListLength(p->vertices[curr]->incoming);   //number of inlinks of the outlink
    }
    
    //if (sum_in == 0) sum_in = 0.5;
    
    return (double)u_in/(double)sum_in;
}

// template function to calculate Wout
// the weight of link(v, u) calculated based on
// the number of outlinks of page u and the number of outlinks
// of all reference pages of page v
static double outLinkW(PageGraph p, int v, int u) {
    // number of outlinks of u
    double u_out = IntListLength(p->vertices[u]->outgoing);
    if (u_out == 0) u_out = 0.5;   //fix up according to spec
    // iterate through pages that v points to and calculate the sum outlinks they have
    double sum_out = 0;
    double inl = IntListLength(p->vertices[v]->outgoing);  //number of outlinks from v
    
    for (int i = 0; i < inl; i++) {     //for each outlink of v
    
        int curr = getIthVal(p->vertices[v]->outgoing, i);  //id of v outlink
        // Avoid issues related to division by 0
        if (IntListLength(p->vertices[curr]->outgoing) == 0) {
            sum_out += 0.5;
        } else {
            sum_out += (double)IntListLength(p->vertices[curr]->outgoing);
        }
    }
    
    //if (sum_out == 0) sum_out = 0.5;
    //printf("outlink of %d: %f sum_out = %f\n", u, (double)u_out/(double)sum_out, sum_out);
    
    return (double)u_out/sum_out;    
}

// template functon to calculate sum of DiffPR values
static double sumDiff(PageGraph g, double *old) {
    double sum = 0;
    int i;
    for (i=0; i<g->nV; i++) {
        sum += fabsl(g->vertices[i]->pagerank - old[i]);
    }
    
    return sum;
}

// helper function to calculate sum of PR_in*W_in*W_out
static double sumPR(int u, PageGraph g, double *old) {
    double sum = 0;
    // list of incoming nodes(URLs) 
    IntList inc = g->vertices[u]->incoming;
    int len = IntListLength(inc);           //number of inlinks
    
    for (int i = 0; i < len; i++) {         //for each inlink of u
        
        int v = getIthVal(inc,i);   //v: page linking in to u
        double in = inLinkW(g, v, u);
        double out = outLinkW(g, v, u);
        sum += old[v]*in*out;
    }
    
    return sum;
}

// calculate wpr values and store them in vertex->pagerank
void pagerank(PageGraph g, double DampingFactor, double DiffPR, int MaxIteration) {
    // store old pr values to calculate new pr values
    double *oldwpr = calloc(g->nV, sizeof(double));
    if (oldwpr == NULL) msgAbort("Failed malloc");
    // initialisation
    for (int i = 0; i < g->nV; i++) {
        
        g->vertices[i]->pagerank = (double)1/(double)g->nV;
        
        oldwpr[i] = g->vertices[i]->pagerank;
    }
    // keep calculating pr for all URLs until exceed one of the limits
    int iteration = 0;
    double diff = DiffPR;   
    while (iteration < MaxIteration && diff >= DiffPR) {
        // calculate pr for all URLs
        for (int i = 0; i < g->nV; i++) {
            g->vertices[i]->pagerank = (1-DampingFactor)/(double)g->nV + DampingFactor*sumPR(i, g, oldwpr);
        }
        diff = sumDiff(g, oldwpr); // update diff
        // update oldwpr
        for (int i = 0; i < g->nV; i++) {
            oldwpr[i] = g->vertices[i]->pagerank;
        }    
        //printDoubles(oldwpr, g->nV);
        iteration++;
    }
    
    free(oldwpr);
}
