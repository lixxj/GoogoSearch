CC=gcc
CFLAGS=-Wall -g -O -std=c11
DEPS=readData.o SList.o graph.o IntList.o StrDict.o strdup.o -lm

all: autotests inverted pagerank inverted searchPagerank searchTfIdf scaledFootrule

#begin executables
autotests: autotests.o $(DEPS)
	$(CC) $(CFLAGS) -o autotests autotests.o $(DEPS)
	
pagerank: pagerank.o $(DEPS)
	$(CC) $(CFLAGS) -o pagerank pagerank.o $(DEPS)
	
inverted: inverted.o $(DEPS)
	$(CC) $(CFLAGS) -o inverted inverted.o $(DEPS)
	
searchPagerank: searchPagerank.o $(DEPS)
	$(CC) $(CFLAGS) -o searchPagerank searchPagerank.o $(DEPS)
	
searchTfIdf: searchTfIdf.o $(DEPS)
	$(CC) $(CFLAGS) -o searchTfIdf searchTfIdf.o $(DEPS)
	
scaledFootrule: scaledFootrule.o $(DEPS)
	$(CC) $(CFLAGS) -o scaledFootrule scaledFootrule.o $(DEPS)
	
#begin object files
	
pagerank.o: pagerank.c
	$(CC) -c $(CFLAGS) pagerank.c
	
inverted.o: inverted.c
	$(CC) -c $(CFLAGS) inverted.c
	
searchPagerank.o: searchPagerank.c
	$(CC) -c $(CFLAGS) searchPagerank.c
	
searchTfIdf.o: searchTfIdf.c
	$(CC) -c $(CFLAGS) searchTfIdf.c
	
scaledFootrule.o: scaledFootrule.c
	$(CC) -c $(CFLAGS) scaledFootrule.c
	
autotests.o: autotests.c
	$(CC) -c $(CFLAGS) autotests.c
	
readData.o: readData.c
	$(CC) -c $(CFLAGS) readData.c
	
graph.o: graph.c graph.h
	$(CC) -c $(CFLAGS) graph.c 
	
SList.o: SList.c SList.h
	$(CC) -c $(CFLAGS) SList.c
	
StrDict.o: StrDict.c StrDict.h
	$(CC) -c $(CFLAGS) StrDict.c
	
IntList.o: IntList.c IntList.h
	$(CC) -c $(CFLAGS) IntList.c
	
strdup.o: strdup.c strdup.h
	$(CC) -c $(CFLAGS) strdup.c
	
clean:
	rm *.o
