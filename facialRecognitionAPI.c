#include "facialRecognitionAPI.h"

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t realsize = size * nmemb;
    get_request *req = (get_request *) userdata;

    while (req->bufferLength < req->length + realsize + 1)
    {
        req->buffer = realloc(req->buffer, req->bufferLength + CHUNK_SIZE);
        req->bufferLength += CHUNK_SIZE;
    }
    memcpy(&req->buffer[req->length], ptr, realsize);

    req->length += realsize;
    req->buffer[req->length] = 0;

    return realsize;
}

int percentageMatch(char* firstPic, char* secondPic) {
    get_request req = {.buffer = NULL, .length = 0, .bufferLength = 0};

    CURL *hnd;
    curl_mime *mime1;
    curl_mimepart *part1;
    struct curl_slist *slist1;

    mime1 = NULL;
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, "x-rapidapi-host: face-verification2.p.rapidapi.com");
    slist1 = curl_slist_append(slist1, "x-rapidapi-key: 367df2b0c2mshf3e275632874e7bp1564fdjsna5916e4e9f5c");

    hnd = curl_easy_init();
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_URL, "https://face-verification2.p.rapidapi.com/FaceVerification");
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    mime1 = curl_mime_init(hnd);
    part1 = curl_mime_addpart(mime1);


    curl_mime_filedata(part1, firstPic); // filename input 1
    curl_mime_name(part1, "photo1");
    part1 = curl_mime_addpart(mime1);

    curl_mime_filedata(part1, secondPic); // filename input 2
    curl_mime_name(part1, "photo2");

    curl_easy_setopt(hnd, CURLOPT_MIMEPOST, mime1);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.64.1");
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(hnd, CURLOPT_HTTP09_ALLOWED, 1L);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

    req.buffer = malloc(CHUNK_SIZE);
    req.bufferLength = CHUNK_SIZE;

    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&req);

    // Execute curl POST request to face-recognition API
    curl_easy_perform(hnd);

    // Clean up hnd
    curl_easy_cleanup(hnd);
    hnd = NULL;
    curl_mime_free(mime1);
    mime1 = NULL;
    curl_slist_free_all(slist1);
    slist1 = NULL;

    // Extract similarity percentage
    cJSON *root = cJSON_Parse(req.buffer);
    cJSON *data = cJSON_GetObjectItem(root, "data");
    cJSON *similarPercent = cJSON_GetObjectItem(data, "similarPercent");

    int percent = 0;
    if (cJSON_IsNumber(similarPercent)) percent = similarPercent->valueint;

    // Free dynamically allocated memory
    free(req.buffer);

    // Return similarity in faces
    return percent;
}