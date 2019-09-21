// Driver Program for pagerank calculation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "readData.h"
#include "graph.h"
#include "IntList.h"

typedef struct _result {
    char *url;
    double pr;
    int nOutlinks;
} result;

// Comparison function for qsort 
// Compare the "pr" in the struct (descending order)
static int cmpfunc(const void *a, const void *b) {
   double p = ((result *)a)->pr;
   double q = ((result *)b)->pr;
   
   if ((p-q) > 0) return -1;
   else     return 1;
}

static void print(result *input, FILE *output, int length) {
	for (int i = 0; i < length; i++){
		fprintf(output, "%s, %d, %.7lf\n", input[i].url, input[i].nOutlinks, input[i].pr);
	}
	return;
}

int main(int argc, char *argv[]) {
    
    // check and read command line arguements
	if(argc != 4){
		fprintf(stderr, "usage: \"./pagerank\" damping_factor diffPR maxIterations");
		return EXIT_FAILURE;
	}
	double DampingFactor, diffPR;
	int MaxIteration;
	DampingFactor = atof(argv[1]);
	diffPR = atof(argv[2]);
	MaxIteration = atoi(argv[3]);
    
    // read from "" and build graph
    SList pages = getPages("collection.txt");
    PageGraph g = buildGraph(pages);
    
    // pagerank calculation
    pagerank(g, DampingFactor, diffPR, MaxIteration);
    
    // extract url names, pagerank values and number of outlinks to array of struct result
    int n = nVertices(g);
    if (n > 0) {    
        result *results = malloc(n*sizeof(struct _result));
        for (int i = 0; i < n; i++) {
            results[i].url = getName(g, i);
            results[i].pr = getPR(g, i);
            results[i].nOutlinks = nOutgoing(g, i);
        }
        
        // sort array of struct result in pagerank desceding order
        qsort(results, n, sizeof(struct _result), cmpfunc);
        
        // print result to "pagerankList.txt"
        FILE *fout = fopen("pagerankList.txt","w");
        print(results, fout, n);
        
        fclose(fout);
        free(results);
    }
    
    // free malloced memory
    free(pages);
	free(g);
	
    return 0;
}
