#include "strList.h"

StrList *initialiseList(void) {
    StrList *newList = malloc(sizeof(StrList));
    newList->start = NULL;
    newList->end = NULL;
    return newList;
}

void freeNode(StrNode *currNode) {
    free(currNode->string);
    free(currNode);
}

void freeList(StrList *list) {
    StrNode *currNode = list->start;
    while (currNode != NULL) {
        StrNode *nodeToRemove = currNode;
        currNode = currNode->next;
        freeNode(nodeToRemove);
    }
    free(list);
}

void addString(StrList *list, char *strToAdd) {
    StrNode *newNode = malloc(sizeof(StrNode));
    assert(newNode != NULL);
    char *newStr = malloc(strlen(strToAdd) + 1);
    assert(newStr != NULL);
    newNode->string = newStr;
    newNode->next = NULL;
    newNode->prev = list->end;
    strcpy(newNode->string, strToAdd);
    if (list->end != NULL) {
        list->end->next = newNode;
    }
    if (list->start == NULL) {
        list->start = newNode;
    }
    list->end = newNode;
    list->size++;
}

void removeString(StrList *list, char *strToRemove) {
    StrNode *listNode = list->start;
    while (listNode != NULL) {
        if (strcmp(listNode->string, strToRemove) == 0) {
            if (listNode->prev != NULL) {
                listNode->prev->next = listNode->next;
            } else {
                list->start = listNode->next;
                if (listNode->next != NULL) {
                    listNode->next->prev = NULL;
                }
            }
            if (listNode->next != NULL) {
                listNode->next->prev = listNode->prev;
            } else {
                list->end = listNode->prev;
                if (listNode->prev != NULL) {
                    listNode->prev->next = NULL;
                }
            }
            freeNode(listNode);
            list->size--;
            return;
        }
        listNode = listNode->next;
    }
}

StrList *copyList(StrList *list) {
    StrList *newList = initialiseList();
    StrNode *listNode = list->start;
    while (listNode != NULL) {
        addString(newList, listNode->string);
        listNode = listNode->next;
    }
    return newList;
}

void printList(StrList *list) {
    StrNode *currNode = list->start;
    int index = 1;
    printf("\n");
    printf("Current size: [%d]\n", list->size);
    printf("====\n");
    while (currNode != NULL) {
        printf("String [%d] is [%s] || \n", index, currNode->string);
        currNode = currNode->next;
        index++;
    }
    printf("====\n");
    printf("\n");
}

StrList *tokenise(char *word, char *delim) {
    StrList *newList = initialiseList();
    while( word != NULL ) {
        addString(newList, word);
        word = strtok(NULL, delim);
    }
    return newList;
}

double matchWords(StrList *list1, StrList *list2) {
    StrNode *currNode = list1->start;
    StrNode *currNode2;
    int matchedPairs = 0;
    int totalPairs = (list1->size < list2->size) ? list1->size : list2->size;
    if(totalPairs == 0){
        return 0;
    }
    while (currNode != NULL) {
        currNode2 = list2->start;
        while (currNode2 != NULL) {
            if (strcmp(currNode->string, currNode2->string) == 0) {
                matchedPairs++;
                removeString(list2, currNode2->string);
                break;
            }
            currNode2 = currNode2->next;
        }
        currNode = currNode->next;
    }
    return (double) matchedPairs / (double) totalPairs;
}