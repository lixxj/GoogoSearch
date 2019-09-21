#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_DISPLAY 30 
#define MAX_SIZE 999

typedef struct _page {
    char url[MAX_SIZE];
    double pr;
    int Nfound; 
} page;

// tenplate function for bubblesort
static void swap(page *xp, page *yp) { 
	page temp = *xp; 
	*xp = *yp; 
	*yp = temp; 
} 

int main(int argc, char **argv) {
	
	// read from "pagerankList.txt" and create pages array to store all pages info
	FILE *fpPR = fopen("pagerankList.txt", "r");
	if(!fpPR){
		fprintf(stderr, "Error opening file\n");
		return -1;
	}
	page pages[MAX_SIZE];
	int ipage = 0;
	char Olinks[10];
	while (fscanf(fpPR,"%s %s %lf",pages[ipage].url,Olinks,&pages[ipage].pr) == 3) {
		int length = strlen(pages[ipage].url);
		pages[ipage].url[length-1] = '\0';
		pages[ipage].Nfound = 0;
		// update ipage
		ipage++;
	}
	fclose(fpPR);
	//for (int h=0; h<ipage; h++) printf("%s\n", pages[h].url);
	
	// read from "invertedIndex.txt" and update Nfound 
	FILE *fpI = fopen("invertedIndex.txt", "r");
    if(!fpI){
        fprintf(stderr, "Error opening file\n");
        return -1;
    }
    char URLs[MAX_SIZE][MAX_SIZE]; // store the (target)URL names contain keyword
    int countURL = 0; 
    char currword[MAX_SIZE];
    while (fscanf(fpI, "%s", currword) != EOF) {
        for (int iword=1; iword<argc; iword++) { // check all keywords
            //printf("currword:%s\n", currword);
            if (strcmp(argv[iword], currword)==0) { // if word is keyword 
                char U[MAX_SIZE]; // string of all URLs seperated by whitespace
	            if (fscanf(fpI, "%[^\n]", U) != 1){} //remove compiler warning
	            // split URLs (by white space) and store in URLs
                int j=0;
                for(int i=0;i<=(strlen(U));i++) {
                    if(U[i]==' ' || U[i]=='\n') { // if space or \n found, assign NULL into newString[ctr]
                        URLs[countURL][j]='\0';
                        countURL++;  //for next URL
                        j=0;    //for next URL, init index to 0
                    } else {
                        URLs[countURL][j]=U[i];
                        j++;
                    }
                }
            } 
        }
        // move to the end of line
        char trash[MAX_SIZE]; 
	    if (fscanf(fpI, "%[^\n]", trash) != 1){} 
	    fgetc(fpI);
    }
    fclose(fpI);
    //for (int h=0; h<countURL; h++) printf("%s\n", URLs[h]);
	
	// iterate through all target URLs and existing URLs, Nfound++ if current URL is a target
    for (int itURL=0; itURL<= countURL; itURL++) {   
        for (int iurl=0; iurl<ipage; iurl++) {
            if (strcmp(pages[iurl].url, URLs[itURL])==0) pages[iurl].Nfound++;
        }
    }
	//for (int j=0; j<ipage; j++) printf("%d\n", pages[j].Nfound);
	
	// already in pr descending order
	// need to sort by Nfound descending order 
	// ***MUST BE STABLE*** ie.won't disturb existing pr order
	// And dear Paul said I'd better write a program to practise
	// so I use this bubble sort to implement
	int I, J; 
    for (I = 0; I < ipage-1; I++) {	 
        // Last I elements are already in place 
	    for (J = 0; J < ipage-I-1; J++) {
		    if (pages[J].Nfound < pages[J+1].Nfound) swap(&pages[J], &pages[J+1]); 
	    }
    } 
	
	// print out result pages
	int displaycount = 0;
	for (int ip=0; ip<ipage; ip++) {
	    if (pages[ip].Nfound > 0 && displaycount < MAX_DISPLAY) {
	        printf("%s\n", pages[ip].url);
	        displaycount++;
        }
	}
	
	return 0;
}
	
