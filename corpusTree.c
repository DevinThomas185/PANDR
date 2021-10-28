#include "corpusTree.h"

CorpusTree *createNode(void) {
    CorpusTree *newNode = (CorpusTree *) calloc (1, sizeof (CorpusTree));
    newNode->children = (CorpusTree **) malloc(ALPHABET_SIZE * sizeof(CorpusTree *));
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        newNode->children[i] = NULL;
    }
    return newNode;
}

void freeCorpusNode(CorpusTree *corpus) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        CorpusTree *childPtr = corpus->children[i];
        if (childPtr != NULL) {
            freeCorpusNode(childPtr);
        }
    }
    if (corpus->stem != NULL) {
        free(corpus->stem);
    }
    free(corpus);  
}

CorpusTree *createCorpus(void) {
    return createNode();
}

void freeCorpus(CorpusTree *corpus) {
    freeCorpusNode(corpus);
}

bool find(CorpusTree *corpus, const char *word, char **stemBuffer) {
    int indexFromChar = ((int) *word) - ((int) 'a');
    if (*word == '\0') {
        if (stemBuffer != NULL) {
            *stemBuffer = corpus->stem;
        }
        return corpus->endOfWord;
    }
    if (indexFromChar < 0 || indexFromChar > 25) {
        return false;
    }
    if (corpus->children[indexFromChar] == NULL) {
        return false;
    }
    word++;
    bool result = find(corpus->children[indexFromChar], word, stemBuffer);
    return result;
}

bool insert(CorpusTree *corpus, const char *word, char *stemToAdd) {
    int indexFromChar = ((int) *word) - ((int) 'a');
    if (*word == '\0') {
        corpus->endOfWord = true;
        char *newStem = malloc(strlen(stemToAdd) + 1);
        strcpy(newStem, stemToAdd);
        corpus->stem = newStem;
        return true;
    }
    if (indexFromChar < 0 || indexFromChar > 25) {
        printf("Index is not in the correct range\n");
        return false;
    }
    if (corpus->children[indexFromChar] == NULL) {
        corpus->children[indexFromChar] = createNode();
    }
    word++;
    bool result = insert(corpus->children[indexFromChar], word, stemToAdd);
    return result;
}

bool loadFromFile(CorpusTree *corpus, const char *filename, bool isStem) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return false;
    }
    fseek(fp, 0L, SEEK_END);
    rewind(fp);
    char lineStr[MAX_WORD_SIZE];
    while (fgets(lineStr, MAX_WORD_SIZE - 1, fp)) {
        bool res;
        if (isStem) {
            char stem[MAX_WORD_SIZE];
            char stemVariants[MAX_WORD_SIZE];
            sscanf(lineStr, "%s %[^\n]s", (char *) stem, (char *) stemVariants);
            res = insert(corpus, stem, stem);
            if (!res) {
                return false;
            }
            stemVariants[strlen(stemVariants)-1] = '\0';
            char *context = NULL;
            int num_operands = 0;
            char *token = strtok_r(stemVariants, ",", &context);
            while (token != NULL)
            {
                res = insert(corpus, token, stem);
                if (!res) {
                    return false;
                }

                num_operands++;
                token = strtok_r(NULL, ",", &context);
            }
            res = true;
        } else {
            lineStr[strcspn(lineStr, "\n")] = 0;
            res = insert(corpus, lineStr, "");
        }
        if (!res) {
            printf("Error could not insert\n");
            return false;
        }
    }
    return true;
}

CorpusTree *loadCorpus(const char *fileName) {
    CorpusTree *newCorpus = createCorpus();
    if (strcmp(fileName, "stems.txt") == 0 || strcmp(fileName, "synonyms.txt") == 0) {
        loadFromFile(newCorpus, fileName, true);
    } else {
        loadFromFile(newCorpus, fileName, false);
    }
    return newCorpus;
}

void printCorpus(CorpusTree *corpus) {
    for (int i = 0; i <= 25; i++) {
        if (corpus->children[i] != NULL) {
            printCorpus(corpus->children[i]);
        }
    }
}

char *getStem(CorpusTree *stemCorpus, char **inputWord) {
    char **stemPtr = malloc(sizeof(char **));
    if (!find(stemCorpus, *inputWord, stemPtr)) {
        return NULL;
    }
    return *stemPtr;
}

StrList *corpusMatches(StrList *words, CorpusTree *corpus) {
    StrList *matches = initialiseList();
    StrNode *currNode = words->start;
    while (currNode != NULL) {
        char *newPtr = currNode->string;
        if (find(corpus, newPtr, NULL)) {
            addString(matches, currNode->string);
        }
        currNode = currNode->next;
    }
    return matches;
}

void groupify(CorpusTree *corpus, StrList *list) {
    StrNode *currNode = list->start;
    while (currNode != NULL) {
        char *empty = currNode->string;
        char **group = &empty;
        char *groupPtr = getStem(corpus, group);
        if (groupPtr != NULL) {
            *currNode->string = *groupPtr;
        }
        currNode = currNode->next;
    }
}

double matchWordGroups(CorpusTree *stemCorpus, StrList *list1, StrList *list2) {
    groupify(stemCorpus, list1);
    groupify(stemCorpus, list2);
    if (list1->size == 0 || list2->size == 0) {
        return 0;
    }
    return matchWords(list1, list2);
}