#ifndef CORPUSTREE_H_ 
#define CORPUSTREE_H_

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "strList.h"

typedef struct CorpusNode {
    struct CorpusNode **children;
    char *stem;
    bool endOfWord;              
} CorpusTree;

CorpusTree *createNode(void);

void freeCorpusNode(CorpusTree *corpus);

CorpusTree *createCorpus(void);

void freeCorpus(CorpusTree *corpus);

bool find(CorpusTree *corpus, const char *word, char **stemBuffer);

bool insert(CorpusTree *corpus, const char *word, char *stemToAdd);

bool loadFromFile(CorpusTree *corpus, const char *filename, bool isStem);

CorpusTree *loadCorpus(const char *fileName);

void printCorpus(CorpusTree *corpus);

char *getStem(CorpusTree *stemCorpus, char **inputWord);

StrList *corpusMatches(StrList *words, CorpusTree *corpus);

void groupify(CorpusTree *stemCorpus, StrList *list);

double matchWordGroups(CorpusTree *stemCorpus, StrList *list1, StrList *list2);

#endif