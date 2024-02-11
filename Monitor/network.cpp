#include<iostream>

#include <curl/curl.h>
//#include "network.h"

using namespace std;

//==============================upload===================================
size_t Write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    return size * nmemb;
}

bool upload_file(const char* host, const char* file_path, const char* dateModified, const char* campain)
{
    CURL* curl;
    CURLcode res;
    FILE* file;
    struct curl_httppost* formpost = NULL;
    struct curl_httppost* lastptr = NULL;
    struct curl_slist* headerlist = NULL;
    static const char buf[] = "Expect:";

    curl_global_init(CURL_GLOBAL_ALL);

    /* Fill in the file upload field */
    curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file", CURLFORM_FILE, file_path, CURLFORM_END);

    curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "new_filename", CURLFORM_COPYCONTENTS, dateModified, CURLFORM_END);
    curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "campain", CURLFORM_COPYCONTENTS, campain, CURLFORM_END);

    curl = curl_easy_init();
    if (curl) {
        /* First set the URL that is about to receive our POST. */
        curl_easy_setopt(curl, CURLOPT_URL, host);

        /* Now specify we want to POST data */
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        /* Add the form data to the request */
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        /* Set the expected response type */
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip, deflate, br");

        /* Set the callback function to receive the response */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Write_callback);

        /* Set the buffer for the response data */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);

        /* Set up a list of custom headers */
        headerlist = curl_slist_append(headerlist, buf);
        headerlist = curl_slist_append(headerlist, "Content-Type: multipart/form-data");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK)
            cerr << "Error uploading file: " << curl_easy_strerror(res) << endl;

        /* Free the custom headers */
        curl_slist_free_all(headerlist);

        /* Free the form data */
        curl_formfree(formpost);

        /* Cleanup curl stuff */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return (res == CURLE_OK);
}