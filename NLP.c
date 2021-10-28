#include "NLP.h"
#include "corpusTree.h"

double runNlpProcess(char *str1, char *str2) {
    printf("str1: (%s), str2: (%s)\n", str1, str2);
    if (strcmp(str1, str2) == 0) {
        return 100;
    }

    CorpusTree *nounCorpus = loadCorpus("nouns.txt");
    CorpusTree *adjectiveCorpus = loadCorpus("adjectives.txt");
    CorpusTree *stemCorpus = loadCorpus("stems.txt");
    CorpusTree *synonymCorpus = loadCorpus("synonyms.txt");

    char delims[2] = " ";
    
    char *tokenPtr1 = strtok(str1, delims);
    StrList *userDesc = tokenise(tokenPtr1, delims);

    char *tokenPtr2 = strtok(str2, delims);
    StrList *dbDesc = tokenise(tokenPtr2, delims);

    StrList *udNouns = corpusMatches(userDesc, nounCorpus);
    StrList *udAdjectives = corpusMatches(userDesc, adjectiveCorpus);

    StrList *dbNouns = corpusMatches(dbDesc, nounCorpus);
    StrList *dbAdjectives = corpusMatches(dbDesc, adjectiveCorpus);


    double nounMatches = matchWords(udNouns, dbNouns);
    double adjMatches = matchWords(udAdjectives, dbAdjectives);
    double stemMatches = matchWordGroups(stemCorpus, userDesc, dbDesc);
    double synonymMatches = matchWordGroups(synonymCorpus, userDesc, dbDesc);
    
    freeList(userDesc);
    freeList(dbDesc);
    freeList(udNouns);
    freeList(udAdjectives);
    freeList(dbNouns);
    freeList(dbAdjectives);
    freeCorpus(nounCorpus);
    freeCorpus(adjectiveCorpus);
    freeCorpus(stemCorpus);
    freeCorpus(synonymCorpus);

    return ((0.2 * nounMatches) + (0.2 * adjMatches) +  (0.3 * stemMatches) + (0.3 * synonymMatches)) * 100;
}