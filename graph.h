#ifndef GRAPH_H
#define GRAPH_H

typedef struct _graphRep *PageGraph;

PageGraph newPageGraph(int nV);
void freePageGraph(PageGraph g);

int addEdge(PageGraph g, char *u, char *v);	// return 1 if new vertex names, otherwise 0. ALLOWS MULTIPLE EDGES
int isEdge(PageGraph g, char *u, char *v);

int nVertices(PageGraph g);
int nOutgoing(PageGraph g, int i);
double getPR(PageGraph g, int i);
char* getName(PageGraph g, int i);
void showPageGraph(PageGraph g);
void pagerank(PageGraph g, double DampingFactor, double DiffPR, int MaxIteration);

#endif
