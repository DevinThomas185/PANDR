#ifndef STRLIST_H_ 
#define STRLIST_H_ 

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ALPHABET_SIZE 26
#define MAX_WORD_SIZE 80

typedef struct StrNode {
    char *string; 
    struct StrNode *next; 
    struct StrNode *prev;
} StrNode;

typedef struct StrList {
    StrNode *start;
    StrNode *end;
    int size;
} StrList;

StrList *initialiseList(void);

void freeNode(StrNode *currNode);

void freeList(StrList *list);

void addString(StrList *list, char *strToAdd);

void removeString(StrList *list, char *strToRemove);

StrList *copyList(StrList *list);

void printList(StrList *list);

StrList *tokenise(char *word, char *delim);

double matchWords(StrList *list1, StrList *list2);

#endif