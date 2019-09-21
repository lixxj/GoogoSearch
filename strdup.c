#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strdup.h"

char *strdup(char *s) {
    
    char *new = malloc(strlen(s)+1);
    if (new == NULL) abort();
    strcpy(new, s);
    return new;
}

