#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "gh_client.h"

#define GH_REQ_JSON_HEADER   "Accept: application/vnd.github+json"
#define GH_REQ_VER_HEADER    "X-GitHub-Api-Version: 2022-11-28"
#define GH_REQ_DEF_UA_HEADER "User-Agent: bd-gh-c-lib"

static CURL *curl = NULL;
static char *token = NULL;

int
gh_client_init(const char *tkn)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        // handle error
        return 1;
    }

    token = tkn;

    return 0;
}

void
gh_client_response_free(gh_client_response_t *res)
{
    if (res != NULL) {
        if (res->resp != NULL) {
            free(res->resp);
        }
        if (res->err_msg != NULL) {
            free(res->err_msg);
        }

        free(res);
    }
}

/**
 * @brief Write a received response into a response type.
 * 
 * @param data 
 * @param size 
 * @param nmemb 
 * @param clientp 
 * @return size_t 
 */
static size_t
cb(char *data, size_t size, size_t nmemb, void *clientp)
{
    size_t realsize = size * nmemb;
    gh_client_response_t *mem = (gh_client_response_t*)clientp;

    char *ptr = realloc(mem->resp, mem->size + realsize + 1);

    mem->resp = ptr;
    memcpy(&(mem->resp[mem->size]), data, realsize);
    mem->size += realsize;
    mem->resp[mem->size] = 0;

    return realsize;
}

/**
 * @brief Create and return a new pointer for response data.
 * 
 * @return gh_client_response_t* 
 */
static gh_client_response_t*
gh_client_response_new()
{
    return calloc(1, sizeof(gh_client_response_t));
}

gh_client_response_t*
gh_client_octocat_says()
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[256];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, "X-GitHub-Api-Version: 2022-11-28");
    chunk = curl_slist_append(chunk, "User-Agent: bd-gh-c-lib");

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/octocat");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    curl_slist_free_all(chunk);

    response->resp_code = res;

    return response;
}

gh_client_response_t*
gh_client_repo_releases_list(const char *owner, const char *repo)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[256];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, "Accept: application/vnd.github+json");
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, "X-GitHub-Api-Version: 2022-11-28");
    chunk = curl_slist_append(chunk, "User-Agent: bd-gh-c-lib");

    char *url = calloc(2048, sizeof(char));
    strcpy(url, "https://api.github.com/repos/");
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/releases");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    curl_slist_free_all(chunk);
    free(url);

    response->resp_code = res;

    return response;
}

gh_client_response_t*
gh_client_repo_releases_create(const char *owner, const char *repo,
    const char *data)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[256];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, "Accept: application/vnd.github+json");
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, "X-GitHub-Api-Version: 2022-11-28");
    chunk = curl_slist_append(chunk, "User-Agent: bd-gh-c-lib");

    char *url = calloc(2048, sizeof(char));
    strcpy(url, "https://api.github.com/repos/");
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/releases");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    curl_slist_free_all(chunk);
    free(url);

    response->resp_code = res;

    return response;
}

gh_client_response_t*
gh_client_repo_branches_list(const char *owner, const char *repo)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[256];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, "Accept: application/vnd.github+json");
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, "X-GitHub-Api-Version: 2022-11-28");
    chunk = curl_slist_append(chunk, "User-Agent: bd-gh-c-lib");

    char *url = calloc(2048, sizeof(char));
    strcpy(url, "https://api.github.com/repos/");
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/branches");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    curl_slist_free_all(chunk);
    free(url);

    response->resp_code = res;

    return response;
}

gh_client_response_t*
gh_client_repo_pull_request_list(const char *owner, const char *repo,
    gh_client_pull_req_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[256];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(2048, sizeof(char));
    strcpy(url, "https://api.github.com/repos/");
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/pulls");

    if (opts != NULL) {
        int first_param_set = 0;
        // set the list state. api def is open
        if (opts->state == GH_PR_STATE_CLOSED) {
            strcat(url, "?state=closed");
            first_param_set = 1;
        } else if (opts->state == GH_PR_STATE_MERGED) {
            strcat(url, "?state=merged");
            first_param_set = 1;
        }

        // set the list order. api def is desc
        if (opts->order == GH_PR_ORDER_ASC) {
            if (first_param_set) {
                strcat(url, "&direction=asc");
            }
            strcat(url, "?direction=asc");
        }
    }


    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    curl_slist_free_all(chunk);
    free(url);

    response->resp_code = res;

    return response;
}

gh_client_response_t*
gh_client_repo_pull_request_get(const char *owner, const char *repo,
    const int unsigned id, gh_client_pull_req_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[256];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(2048, sizeof(char));
    strcpy(url, "https://api.github.com/repos/");
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/pulls/");

    char id_val[11] = {0};
    sprintf(id_val, "%d", id);
    strcat(url, id_val);

    if (opts != NULL) {
        // set the list state. api def is open
        if (opts->state == GH_PR_STATE_CLOSED) {
            strcat(url, "?state=closed");
        } else if (opts->state == GH_PR_STATE_MERGED) {
            strcat(url, "?state=merged");
        }
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    curl_slist_free_all(chunk);
    free(url);

    response->resp_code = res;

    return response;
}

gh_client_response_t*
gh_client_user_logged_in_get()
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[256];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(2048, sizeof(char));
    strcpy(url, "https://api.github.com/user");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    curl_slist_free_all(chunk);
    free(url);

    response->resp_code = res;

    return response;
}

gh_client_response_t*
gh_client_user_by_id_get(const char *username)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[256];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(2048, sizeof(char));
    strcpy(url, "https://api.github.com/users/");
    strcat(url, username);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    curl_slist_free_all(chunk);
    free(url);

    response->resp_code = res;

    return response;
}

gh_client_response_t*
gh_client_user_by_id_hovercard_get(const char *username)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[256];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(2048, sizeof(char));
    strcpy(url, "https://api.github.com/users/");
    strcat(url, username);
    strcat(url, "/hovercard");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    curl_slist_free_all(chunk);
    free(url);

    response->resp_code = res;

    return response;
}

void
gh_client_free()
{
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}
