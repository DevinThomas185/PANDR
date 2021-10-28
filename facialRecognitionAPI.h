#ifndef FACIALRECOGNITION_H_
#define FACIALRECOGNITION_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

#define CHUNK_SIZE 2048

typedef struct {
    char *buffer;
    size_t length;
    size_t bufferLength;
} get_request;



int percentageMatch(char* firstPic, char* secondPic);

#endif