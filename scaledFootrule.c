#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h> 
#include "StrDict.h"
#include "SList.h"

#define ZERO_TOL 1E-9

typedef struct _elem {
    
    char *name;
    double *places;
    int nPrefs;
}elem;

/*static void printPrefs(elem *prefs, int nmemb) {*/

/*    for (int p = 0; p < nmemb; p++) {*/
/*    */
/*        printf("%s\t", prefs[p].name);*/
/*        for (int n = 0; n < nmemb; n++) {*/
/*            */
/*            printf("%.4f\t", prefs[p].places[n]);*/
/*        }*/
/*        printf("\n");*/
/*    }*/
/*}*/

/*static void printInts(int arr[], int nmemb) {*/

/*    for (int i = 0; i < nmemb; i++)*/
/*        printf("%d\t", arr[i]);*/
/*    printf("\n");*/
/*}*/

/*static void printElemStr(elem *e, int nmemb) {*/
/*    */
/*    for (int i = 0; i < nmemb; i++)*/
/*        printf("%s\t", e[i].name);*/
/*    printf("\n");*/
/*}*/

static int fZero(double r) {
//works as a better comparision of a double to zero, due to floating-point errors
    return fabs(r) <= ZERO_TOL;
}

static double scaledFoot(char *page, int pos, StrDict *ranks, int nRanks, int cSize) {
//calculate W(c, p) sum
    double sum = 0;
    for (int i = 0; i < nRanks; i++) {
    
        int rank = lookup(ranks[i], page);  //T_i(c)
        if (rank == -1) continue;    //not in this rank list
        
        sum += fabs((double)rank/(double)nEntries(ranks[i]) - (double)pos/(double)cSize);
    }
    return sum;
}

static void reduceRows(elem *prefs, int nPages) {
//subtract the minimal value in each row from each row element
    
    for (int p = 0; p < nPages; p++) {  //for each row
        
        double min = prefs[0].places[p];
        
        for (int i = 1; i < nPages; i++) {  //search for min value
            if (prefs[i].places[p] < min)
                min = prefs[i].places[p];
        }
        
        for (int i = 0; i < nPages; i++)
            prefs[i].places[p] -= min;
    }
}

static void reduceCols(elem *prefs, int nPages) {
//subtract the minimal value in each col from each col element

    for (int n = 0; n < nPages; n++) {   //for each column
        
        double min = prefs[n].places[0];
        
        for (int p = 1; p < nPages; p++) {  //search for min value
            
            double curr = prefs[n].places[p];
            if (curr < min)
                min = curr;
        }
        
        for (int i = 0; i < nPages; i++) {
            if (prefs[n].places[i] != 0)
                prefs[n].places[i] -= min;
        }
    }
}

static int simpleAssign(elem *prefs, int *final, int nPages) {
//assume prefs is easy to assign

    memset(final, 0, nPages*sizeof(*final));
    int nMatched = 0;
    int prog = 1;
    int *taken = calloc(nPages, sizeof(int));   //array of places: 0 for free, 1 for taken
    
    while (nMatched < nPages && prog) {     //while not all matched
        
        prog = 0;
        //count 0s for each page: if one has only one, allocate it
        for (int page = 0; page < nPages; page++) {
            
            int nZeroes = 0;
            int zPos = 0;
            
            for (int pos = 0; pos < nPages; pos++) {
                
                if (prefs[page].places[pos] == 0) {
                    nZeroes++;
                    zPos = pos;
                }
                if (nZeroes > 1)    break;  //exceeded
            }
            if (nZeroes == 1 && !taken[zPos]) { //if found single 0
                final[page] = zPos+1;
                taken[zPos] = 1;
                nMatched++;
                prog = 1;
            }
        }
        //regular matching
        for (int p = 0; p < nPages; p++) {
            for (int n = 0; n < nPages; n++) {   //for each page
                if (prefs[n].places[p] == 0 && final[n] == 0 && !taken[p]) {   //if potential match
                
                    final[n] = p+1;
                    taken[p] = 1;
                    nMatched++;
                    p = nPages;
                    prog = 1;
                    break;
                }
            }
        }
    }
    free(taken);
    return !prog;
}

int sumVal(int arr[], int val, int len) {

    int count = 0;
    for (int i = 0; i < len; i++)
        if (arr[i] == val) count++;
        
    return count;
}

static void hungary(elem *prefs, int *final, int nPages) {
//requires already row-reduced rows/cols
//perform Hungarian algorithm for optimal assignment of positions to pages

    int *hZeros = malloc(nPages*sizeof(int));   //count of zeros in rows
    int *vZeros = malloc(nPages*sizeof(int));   //count of zeros in columns
    
    int *hLines = malloc(nPages*sizeof(int));   //horizontal line tracker
    int *vLines = malloc(nPages*sizeof(int));   //vertical line tracker
    memset(final, 0, nPages*sizeof(*final));
    
    while (1) {
    
        memset(hZeros, 0, nPages*sizeof(int));
        memset(vZeros, 0, nPages*sizeof(int));
        memset(hLines, 0, nPages*sizeof(int));
        memset(vLines, 0, nPages*sizeof(int));
        
        int nZeros = 0;
        //count zeroes in each row and column
        for (int r = 0; r < nPages; r++) {
        
            for (int c = 0; c < nPages; c++) {
                
                if (fZero(prefs[r].places[c])) {
                    hZeros[r]++;
                    vZeros[c]++;
                    nZeros++;
                }
            }
        }
        //set horizontal/vertical lines
        while (nZeros > 0) {
        
            int axis = 0;
            int maxIndex = 0;
            int max = 0;
            
            for (int h = 0; h < nPages; h++) {
                if (hZeros[h] > max) {  //found new max
                    maxIndex = h;
                    max = hZeros[h];
                    axis = 1;
                }
            }
            for (int v = 0; v < nPages; v++) {
                if (vZeros[v] > max) {  //found new max
                    maxIndex = v;
                    max = vZeros[v];
                    axis = 0;
                }
            }
            
            //intf("Got max #zero=%d on axis %d, index %d\n",max, axis, maxIndex);
            
            //attempt to cover all 0s in the prefs matrix with the least horizontal and vertical lines.
            //axis=0 -> vertical line will be drawn.
            //axis=1 --> horizontal line will be drawn.
            if (axis == 0) {    //if max is on the vertical
                for (int row = 0; row < nPages; row++) {    //remove 1 from 
                                                            //row counts which have a 0 in the column
                    if (fZero(prefs[row].places[maxIndex]))
                        hZeros[row]--;
                }
                nZeros -= vZeros[maxIndex];
                vZeros[maxIndex] = 0;
                vLines[maxIndex] = 1;
            }else{          //max is on the horizontal
                for (int col = 0; col < nPages; col++) {    //remove 1 from 
                                                            //col counts which have a 0 in the row
                    if (fZero(prefs[maxIndex].places[col]))
                        vZeros[col]--;
                }
                nZeros -= hZeros[maxIndex];
                hZeros[maxIndex] = 0;
                hLines[maxIndex] = 1;
            }
            //printf("Have %d zeros\n", nZeros);
        }
            
        int sum = sumVal(hLines, 1, nPages) + sumVal(vLines, 1, nPages);    //get number of lines drawn
        //printf("%d\n", sum);
        
        if (sum == nPages) {
        //matrix is easy to assign; hand over to simpleAssign and exit
            /*printf("BIG yeet\n");
            printf("h lines: ");
            printInts(hLines, nPages);
            printf("v lines: ");
            printInts(vLines, nPages);
            printPrefs(prefs, nPages);*/
            simpleAssign(prefs, final, nPages);
            //clean up
            free(hZeros);
            free(vZeros);
            free(hLines);
            free(vLines);
            return;
        }
        
        /*printf("\n");
        printPrefs(prefs, nPages);
        printf("\nh counts: ");
        printInts(hZeros, nPages);
        printf("v counts: ");
        printInts(vZeros, nPages);
        printf("h lines: ");
        printInts(hLines, nPages);
        printf("v lines: ");
        printInts(vLines, nPages);*/
        double min = 9999999;   //large enough for purpose
        
        //find minimum in cells with no line across them
        for (int e = 0; e < nPages; e++) {  //for each column (p pos) 
            
            if (vLines[e] == 0) {      //if found an unmarked column
                for (int m = 0; m < nPages; m++) {  //for each row of the column
                    
                    if (hLines[m] == 0 && prefs[m].places[e] < min)
                        min = prefs[m].places[e];
                }
            }
        }
        //printf("min unmarked value is %f\n", min);
        
        //subtraction/addition step of Hungarian algo
        for (int c = 0; c < nPages; c++) {  //for each column
        
            for (int r = 0; r < nPages; r++) {  //for each row
                
                if (vLines[c] == 1) {
                    if (hLines[r] == 1)  //if covered by 2 lines
                        prefs[r].places[c] += min;

                }else if (hLines[r] == 0) {   //if uncovered
                    prefs[r].places[c] -= min;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s {rank list}\n", argv[0]);
        exit(1);
    }

    int nRanks = argc-1;    //number of rank lists

    StrDict *ranks = calloc(sizeof(StrDict), argc-1); //page-position dictionary (T_i corresponds to rank[i-1])
    
    for (int i = 0; i < nRanks; i++) {
        ranks[i] = newStrDict();
    }

    SList pages = newSList();
    int file = 1;
    while (file <= nRanks) {   //for each rank file

        FILE *rankf = fopen(argv[file], "r");
        if (rankf == NULL) {
            fprintf(stderr, "Could not open file %s\n", argv[file]);
            abort();
        }
        char lineBuf[512] = {0};
        int line = 1;
        //printf("opening %s\n", argv[file]);
        while (lineBuf == fgets(lineBuf, 512, rankf)) {    //for each page in the rank
        
            lineBuf[strlen(lineBuf)-1] = '\0';      //strip newline
            pages = addStr(pages, lineBuf);         //keep track of added pages
            ranks[file-1] = addIndex(ranks[file-1], line, lineBuf); //add page to the corresponding rank dictionary
            line++;
        }
        fclose(rankf);
        file++;
    }
    
    int nPages = nStr(pages);
    elem *prefs = calloc(sizeof(elem), nPages);    //array of each page and their W(c, p) values
    if (prefs == NULL) abort();
    
    for (int a = 0; a < nPages; a++) {
        
        pages = popFirstStr(pages, &(prefs[a].name));    //add name to preference
        prefs[a].places = calloc(sizeof(double), nPages);   //allocate W(c,p) value array for p
        if (prefs[a].places == NULL) abort();
    }
    
    //PREFERENCE: position where W(c, p) is minimal
    //we will now iterate through each url from 0 to nPages-1, and compute the weight for each p value
    
   for (int i = 0; i < nPages; i++) {
        
        for (int p = 0; p < nPages; p++) {
            
            prefs[i].places[p] = scaledFoot(prefs[i].name, p+1, ranks, nRanks, nPages);
            prefs[i].nPrefs++;
        }
    }
    
    //following: Hungarian algorithm
    reduceRows(prefs, nPages);
    reduceCols(prefs, nPages);
    //printPrefs(prefs, nPages);
    
    int *final = malloc(nPages*sizeof(int));
    
    hungary(prefs, final, nPages);
    //printElemStr(prefs, nPages);
    //printInts(final, nPages);
    
    //final output: sum of distances, followed by pages in order
    //print weight sum
    double sum = 0;
    for (int sfr = 0; sfr < nPages; sfr++)
        sum+=scaledFoot(prefs[sfr].name, final[sfr], ranks, nRanks, nPages);
    printf("%.6f\n", sum);
    
    //print ordered output (i realise this is rather inefficient (O(nPages^2)), 
    //but it's overtaken by hungarian complexity so it doesn't really matter
    for (int print = 1; print <= nPages; print++) {
        
        int next = 0;
        while (final[next] != print)    next++;
        printf("%s\n", prefs[next].name);
    }
      
    for (int i = 0; i < nRanks; i++)    //clean up
        freeStrDict(ranks[i]);
    
    for (int d = 0; d < nPages; d++) {
        free(prefs[d].name);
        free(prefs[d].places);
    }
    free(final);
    free(prefs);
    free(ranks);
        
    return EXIT_SUCCESS;
}
