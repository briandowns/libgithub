/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Brian J. Downs
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define _DEFAULT_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "github.h"

#define GH_REQ_JSON_HEADER   "Accept: application/vnd.github+json"
#define GH_REQ_VER_HEADER    "X-GitHub-Api-Version: 2022-11-28"
#define GH_REQ_DEF_UA_HEADER "User-Agent: bd-gh-c-lib"

#define TOKEN_HEADER_SIZE 256
#define DEFAULT_URL_SIZE  2048

#define SET_BASIC_CURL_CONFIG \
    curl_easy_setopt(curl, CURLOPT_URL, url); \
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); \
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk); \
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb); \
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

#define CALL_CLEANUP \
    curl_slist_free_all(chunk); \
    free(url);

#define CURL_CALL_ERROR_CHECK \
    if(res != CURLE_OK) { \
        char *err_msg = (char *)curl_easy_strerror(res); \
        response->err_msg = calloc(strlen(err_msg)+1, sizeof(char)); \
        strcpy(response->err_msg, err_msg); \
        CALL_CLEANUP; \
        return response; \
    }

static CURL *curl = NULL;
static char *token = NULL;

int
gh_client_init(const char *tkn)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        return 1;
    }

    token = (char *)tkn;

    return 0;
}

void
gh_client_response_free(gh_client_response_t *res)
{
    if (res != NULL) {
        if (res->resp != NULL) free(res->resp);
        if (res->err_msg != NULL) free(res->err_msg);
        if (res->first_link != NULL) free(res->first_link);
        if (res->next_link != NULL) free(res->next_link);
        if (res->prev_link != NULL) free(res->prev_link);
        if (res->last_link != NULL) free(res->last_link);

        if (res->rate_limit_data != NULL) {
            if (res->rate_limit_data->resource != NULL) {
                free(res->rate_limit_data->resource);
            }
            free(res->rate_limit_data);
        }

        free(res);
    }
}

/**
 * Write a received response into a response type.
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
 * Trim unnecessary whitespace from the given string. 
 */
static char*
trim_whitespace(char *str)
{
    while (isspace((unsigned char)*str)) {
        str++;
    }
    if (*str == 0) {
        return str;
    }

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';

    return str;
}

typedef struct {
    char *url;
    char *rel;
} link_t;

/**
 * Parse out the URLs and info from the link header.
 */
static int
parse_link_header(const char *header, link_t *links, int count)
{
    int link_count = 0;
    char *token = strtok((char *)header, ",");

    while (token != NULL && link_count < count) {
        char *url_start = strchr(token, '<');
        char *url_end = strchr(token, '>');
        char *rel_start = strstr(token, "rel=\"");
        char *rel_end = strchr(rel_start, '\"');

        if (url_start && url_end && rel_start && rel_end) {
            *url_end = '\0';
            *rel_end = '\0';

            links[link_count].url = strdup(url_start + 1);
            links[link_count].rel = strdup(rel_start + 5);

            link_count++;
        }

        token = strtok(NULL, ",");
    }

    return link_count;
}

/**
 * Process response header information.
 */
size_t
header_cb(char *buffer, size_t size, size_t nmemb, void *userdata)
{
    size_t total_size = size * nmemb;
    gh_client_response_t *response = (gh_client_response_t*)userdata;

    char *line = strtok(buffer, "\r\n");
    char *key = strsep(&line, ":");
    char *value = strsep(&line, "\n");

    if (key != NULL && value != NULL) {
        if (strcmp(key, "x-ratelimit-limit") == 0) {
            char *v = trim_whitespace(value);
            response->rate_limit_data->limit = atoi(v);
        }
        if (strcmp(key, "x-ratelimit-remaining") == 0) {
            char *v = trim_whitespace(value);
            response->rate_limit_data->remaining = atoi(v);
        }
        if (strcmp(key, "x-ratelimit-reset") == 0) {
            char *v = trim_whitespace(value);
            response->rate_limit_data->reset = atoi(v);
        }
        if (strcmp(key, "x-ratelimit-used") == 0) {
            char *v = trim_whitespace(value);
            response->rate_limit_data->used = atoi(v);
        }
        if (strcmp(key, "x-ratelimit-resource") == 0) {
            char *v = trim_whitespace(value);
            response->rate_limit_data->resource = calloc(strlen(v), sizeof(char));
            strcpy(response->rate_limit_data->resource, v);
        }

        if (strcmp(key, "link") == 0) {
            int link_count = 0;
            for (int i = 0; value[i]; i++) {
                if (value[i] == ',') {
                    link_count++;
                }
            }
            if (link_count > 0) {
                link_count++;
            }

            link_t links[link_count];
            parse_link_header(value, links, link_count);

            for (int i = 0; i < link_count; i++) {
                if (strcmp(links[i].rel, "first\"") == 0) {
                    response->first_link = calloc(strlen(links[i].url)+1, sizeof(char));
                    strcpy(response->first_link, links[i].url);
                }
                if (strcmp(links[i].rel, "prev\"") == 0) {
                    response->prev_link = calloc(strlen(links[i].url)+1, sizeof(char));
                    strcpy(response->prev_link, links[i].url);
                }
                if (strcmp(links[i].rel, "next\"") == 0) {
                    response->next_link = calloc(strlen(links[i].url)+1, sizeof(char));
                    strcpy(response->next_link, links[i].url);
                }

                if (strcmp(links[i].rel, "last\"") == 0) {
                    response->last_link = calloc(strlen(links[i].url)+1, sizeof(char));
                    strcpy(response->last_link, links[i].url);
                }

                free(links[i].url);
                free(links[i].rel);
            }
        }
    }

    return total_size;
}

/**
 * Create and return a new pointer for response data.
 */
static gh_client_response_t*
gh_client_response_new()
{
    gh_client_response_t *resp = calloc(1, sizeof(gh_client_response_t));
    resp->rate_limit_data = calloc(1, sizeof(gh_client_rate_limit_data_t));

    return resp;
}

gh_client_response_t*
gh_client_octocat_says()
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = GH_API_BASE_URL "/octocat";
    SET_BASIC_CURL_CONFIG;

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);

    if (res != CURLE_OK) {
        char *err_msg = (char *)curl_easy_strerror(res);
        response->err_msg = calloc(strlen(err_msg)+1, sizeof(char));
        strcpy(response->err_msg, err_msg);

        curl_slist_free_all(chunk);

        return response;
    }
    curl_slist_free_all(chunk);

    return response;
}

gh_client_response_t*
gh_client_repo_releases_list(const char *owner, const char *repo,
                             const gh_client_req_list_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));

    if (opts != NULL && opts->page_url != NULL) {
        strcpy(url, opts->page_url);
    } else {
        strcpy(url, GH_API_REPO_URL);
        strcat(url, owner);
        strcat(url, "/");
        strcat(url, repo);
        strcat(url, "/releases");
    }

    if (opts != NULL && opts->per_page > 30) {
        strcat(url, "?per_page=");

        char pp_val[11] = {0};
        sprintf(pp_val, "%d", opts->per_page);
        strcat(url, pp_val);
    }

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, response);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;
    
    return response;
}

gh_client_response_t*
gh_client_repo_releases_latest(const char *owner, const char *repo)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/releases/latest");

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_release_by_tag(const char *owner, const char *repo,
                              const char *tag)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/releases/tags/");
    strcat(url, tag);

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_release_by_id(const char *owner, const char *repo,
                             const unsigned int id)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/releases/");

    char id_val[11] = {0};
    sprintf(id_val, "%d", id);
    strcat(url, id_val);

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_release_create(const char *owner, const char *repo,
                              const char *data)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/releases");

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);

    CURL_CALL_ERROR_CHECK;
    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_release_update(const char *owner, const char *repo,
                              const unsigned int id, const char *data)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/releases/");

    char id_val[11] = {0};
    sprintf(id_val, "%d", id);
    strcat(url, id_val);

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);

    CURL_CALL_ERROR_CHECK;
    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_release_delete(const char *owner, const char *repo,
                              const unsigned int id)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/releases/");

    char id_val[11] = {0};
    sprintf(id_val, "%d", id);
    strcat(url, id_val);

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);

    CURL_CALL_ERROR_CHECK;
    CALL_CLEANUP;

    return response; 
}

gh_client_response_t*
gh_client_repo_release_gen_notes(const char *owner, const char *repo,
                                 const char *data)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/releases/generate-notes");

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_release_assets_list(const char *owner, const char *repo,
                                   const unsigned int id,
                                   const gh_client_req_list_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));

    if (opts != NULL && opts->page_url != NULL) {
        strcpy(url, opts->page_url);
    } else {
        strcpy(url, GH_API_REPO_URL);
        strcat(url, owner);
        strcat(url, "/");
        strcat(url, repo);
        strcat(url, "/releases/");

        char id_val[11] = {0};
        sprintf(id_val, "%d", id);
        strcat(url, id_val);
        strcat(url, "/assets");
    }

    if (opts != NULL && opts->per_page > 30) {
        strcat(url, "?per_page=");

        char pp_val[11] = {0};
        sprintf(pp_val, "%d", opts->per_page);
        strcat(url, pp_val);
    }

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_release_asset_get(const char *owner, const char *repo,
                                  const unsigned int id)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/releases/assets/");

    char id_val[11] = {0};
    sprintf(id_val, "%d", id);
    strcat(url, id_val);


    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response; 
}

gh_client_response_t*
gh_client_repo_commits_list(const char *owner, const char *repo,
                            const gh_client_commits_list_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    if (opts != NULL && opts->page_url != NULL) {
        strcpy(url, opts->page_url);
    } else {
        strcpy(url, GH_API_REPO_URL);
        strcat(url, owner);
        strcat(url, "/");
        strcat(url, repo);
        strcat(url, "/commits");
    }

    if (opts != NULL) {
        int first_param_set = 0;

        if (opts->sha != NULL) {
            first_param_set ? strcat(url, "&sha="), strcat(url, opts->sha):
                strcat(url, "?sha="), strcat(url, opts->sha);           
        }
        if (opts->path != NULL) {
            first_param_set ? strcat(url, "&path="), strcat(url, opts->path):
                strcat(url, "?path="), strcat(url, opts->path);
        }
        if (opts->author != NULL) {
            first_param_set ? strcat(url, "&author="),
                strcat(url, opts->author):
                strcat(url, "?author="), strcat(url, opts->author);
        }
        if (opts->committer != NULL) {
            first_param_set ? strcat(url, "&committer="),
                strcat(url, opts->committer):
                strcat(url, "?committer="), strcat(url, opts->committer);
        }
        if (opts->since != NULL) {
            first_param_set ? strcat(url, "&since="), strcat(url, opts->since):
                strcat(url, "?since="), strcat(url, opts->since);
        }
        if (opts->until != NULL) {
            first_param_set ? strcat(url, "&until="), strcat(url, opts->until):
                strcat(url, "?until="), strcat(url, opts->until);
        }
        if (opts->per_page > 30) {
            char pp_val[11] = {0};
            first_param_set ? strcat(url, "&per_page="),
                sprintf(pp_val, "%d", opts->per_page), strcat(url, pp_val):
                strcat(url, "?per_page="),
                sprintf(pp_val, "%d", opts->per_page),
                strcat(url, pp_val);
        }
    }

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_pr_commits_list(const char *owner, const char *repo,
                               const char *sha,
                               const gh_client_req_list_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    
    if (opts != NULL && opts->page_url != NULL) {
        strcpy(url, opts->page_url);
    } else {
        strcpy(url, GH_API_REPO_URL);
        strcat(url, owner);
        strcat(url, "/");
        strcat(url, repo);
        strcat(url, "/commits");
        strcat(url, "/");
        strcat(url, sha);
        strcat(url, "/pulls");
    }

    if (opts != NULL && opts->per_page > 30) {
        strcat(url, "?per_page=");

        char pp_val[11] = {0};
        sprintf(pp_val, "%d", opts->per_page);
        strcat(url, pp_val);
    }

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_commit_get(const char *owner, const char *repo,
                          const char *sha)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/commits");
    strcat(url, "/");
    strcat(url, sha);

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_commits_compare(const char *owner, const char *repo,
                               const char *base, const char *head)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/compare");
    strcat(url, "/");
    strcat(url, base);
    strcat(url, "...");
    strcat(url, base);

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;  
}

gh_client_response_t*
gh_client_repo_branches_list(const char *owner, const char *repo,
                             const gh_client_req_list_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    
    if (opts != NULL && opts->page_url != NULL) {
        strcpy(url, opts->page_url);
    } else {
        strcpy(url, GH_API_REPO_URL);
        strcat(url, owner);
        strcat(url, "/");
        strcat(url, repo);
        strcat(url, "/branches");
    }

    if (opts != NULL && opts->per_page > 30) {
        strcat(url, "?per_page=");

        char pp_val[11] = {0};
        sprintf(pp_val, "%d", opts->per_page);
        strcat(url, pp_val);
    }

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_branch_get(const char *owner, const char *repo,
                          const char *branch)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/branches/");
    strcat(url, branch);

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_branch_rename(const char *owner, const char *repo,
                             const char *branch, const char *data)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/branches/");
    strcat(url, branch);
    strcat(url, "/rename");

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_branch_sync_upstream(const char *owner, const char *repo,
                                    const char *branch, const char *data)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/branches/");
    strcat(url, branch);
    strcat(url, "/merge-upstream");

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_branch_merge(const char *owner, const char *repo,
                            const char *data)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/merges");

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_pull_request_list(const char *owner, const char *repo,
                                 const gh_client_pull_req_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));

    if (opts != NULL && opts->page_url != NULL) {
        strcpy(url, opts->page_url);
    } else {
        strcpy(url, GH_API_REPO_URL);
        strcat(url, owner);
        strcat(url, "/");
        strcat(url, repo);
        strcat(url, "/pulls");
    }

    if (opts != NULL) {
        int first_param_set = 0;

        if (opts->state == GH_ITEM_STATE_CLOSED) {
            strcat(url, "?state=closed");
            first_param_set = 1;
        } else if (opts->state == GH_ITEM_STATE_MERGED) {
            strcat(url, "?state=merged");
            first_param_set = 1;
        }

        // set the list order. api def is desc
        if (opts->order == GH_ORDER_ASC) {
            first_param_set ? strcat(url, "&direction=asc"):
                strcat(url, "?direction=asc");
        }
    }

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_repo_pull_request_get(const char *owner, const char *repo,
                                const int unsigned id,
                                const gh_client_pull_req_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/pulls/");

    char id_val[11] = {0};
    sprintf(id_val, "%d", id);
    strcat(url, id_val);

    if (opts != NULL) {
        // set the list state. api def is open
        if (opts->state == GH_ITEM_STATE_CLOSED) {
            strcat(url, "?state=closed");
        } else if (opts->state == GH_ITEM_STATE_MERGED) {
            strcat(url, "?state=merged");
        }
    }

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_user_logged_in_get()
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_USER_URL);

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_user_by_id_get(const char *username)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_USERS_URL);
    strcat(url, username);

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_user_by_id_hovercard_get(const char *username)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_USERS_URL);
    strcat(url, username);
    strcat(url, "/hovercard");

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_user_blocked_list(const gh_client_req_list_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    
    if (opts != NULL && opts->page_url != NULL) {
        strcpy(url, opts->page_url);
    } else {
        strcpy(url, GH_API_USER_URL "/blocks");
    }

    if (opts != NULL && opts->per_page > 30) {
        strcat(url, "?per_page=");

        char pp_val[11] = {0};
        sprintf(pp_val, "%d", opts->per_page);
        strcat(url, pp_val);
    }

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_user_blocked_by_id(const char *username)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_USER_URL "/blocks/");
    strcat(url, username);

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response; 
}

gh_client_response_t*
gh_client_user_block_by_id(const char *username)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_USER_URL "/blocks/");
    strcat(url, username);

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); 

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response; 
}

gh_client_response_t*
gh_client_user_unblock_by_id(const char *username)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_USER_URL "/blocks/");
    strcat(url, username);

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE"); 

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_issues_for_user_list(const gh_client_issues_req_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    if (opts != NULL && opts->page_url != NULL) {
        strcpy(url, opts->page_url);
    } else {
        strcpy(url, GH_API_ISSUES_URL);
    }

    if (opts != NULL) {
        int first_param_set = 0;

        if (opts->labels != NULL) {
            first_param_set ? strcat(url, "&labels="),
                strcat(url, opts->labels):
                strcat(url, "?labels="), strcat(url, opts->labels);           
        }
        if (opts->since != NULL) {
            first_param_set ? strcat(url, "&since="), strcat(url, opts->since):
                strcat(url, "?since="), strcat(url, opts->since);
        }
        if (opts->per_page > 30) {
            char pp_val[11] = {0};
            first_param_set ? strcat(url, "&per_page="),
                sprintf(pp_val, "%d", opts->per_page), strcat(url, pp_val):
                strcat(url, "?per_page="),
                sprintf(pp_val, "%d", opts->per_page),
                strcat(url, pp_val);
        }
        if (opts->state == GH_ITEM_STATE_CLOSED) {
            first_param_set ? strcat(url, "&state=closed"):
                strcat(url, "?state=closed");
        } else if (opts->state == GH_ITEM_STATE_ALL) {
            first_param_set ? strcat(url, "?state=all"):
                strcat(url, "?state=all");
        }

        if (opts->filter == GH_ISSUE_FILTER_ALL) {
            first_param_set ? strcat(url, "&filter=all"):
                strcat(url, "?filter=all");
        } else if (opts->filter == GH_ISSUE_FILTER_CREATED) {
            first_param_set ? strcat(url, "?filter=created"):
                strcat(url, "?filter=created");
        } else if (opts->filter == GH_ISSUE_FILTER_MENTIONED) {
            first_param_set ? strcat(url, "?filter=mentioned"):
                strcat(url, "?filter=mentioned");
        } else if (opts->filter == GH_ISSUE_FILTER_SUBSCRIBED) {
            first_param_set ? strcat(url, "?filter=subscribed"):
                strcat(url, "?filter=subscribed");
        } else if (opts->filter == GH_ISSUE_FILTER_REPOS) {
            first_param_set ? strcat(url, "?filter=repos"):
                strcat(url, "?filter=repos");
        }

        if (opts->filter == GH_ISSUE_FILTER_ALL) {
            first_param_set ? strcat(url, "&filter=all"):
                strcat(url, "?filter=all");
        }

        if (opts->order == GH_ORDER_ASC) {
            first_param_set ? strcat(url, "&direction=asc"):
                strcat(url, "?direction=asc");
        }
    }

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_issues_by_repo_list(const char *owner, const char *repo,
                              const gh_client_issues_req_opts_t *opts)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    if (opts != NULL && opts->page_url != NULL) {
        strcpy(url, opts->page_url);
    } else {
        strcpy(url, GH_API_REPO_URL);
        strcat(url, owner);
        strcat(url, "/");
        strcat(url, repo);
        strcat(url, "/issues");
    }

    if (opts != NULL) {
        int first_param_set = 0;

        if (opts->assignee != NULL) {
            first_param_set ? strcat(url, "&assignee="),
                strcat(url, opts->assignee):
                strcat(url, "?assignee="), strcat(url, opts->assignee);           
        }
        if (opts->creator != NULL) {
            first_param_set ? strcat(url, "&creator="),
                strcat(url, opts->creator):
                strcat(url, "?creator="), strcat(url, opts->creator);           
        }
        if (opts->mention != NULL) {
            first_param_set ? strcat(url, "&mention="),
                strcat(url, opts->mention):
                strcat(url, "?mention="), strcat(url, opts->mention);           
        }
        if (opts->labels != NULL) {
            first_param_set ? strcat(url, "&labels="),
                strcat(url, opts->labels):
                strcat(url, "?labels="), strcat(url, opts->labels);           
        }
        if (opts->since != NULL) {
            first_param_set ? strcat(url, "&since="), strcat(url, opts->since):
                strcat(url, "?since="), strcat(url, opts->since);
        }
        if (opts->per_page > 30) {
            char pp_val[11] = {0};
            first_param_set ? strcat(url, "&per_page="),
                sprintf(pp_val, "%d", opts->per_page), strcat(url, pp_val):
                strcat(url, "?per_page="),
                sprintf(pp_val, "%d", opts->per_page),
                strcat(url, pp_val);
        }
        if (opts->state == GH_ITEM_STATE_CLOSED) {
            first_param_set ? strcat(url, "&state=closed"):
                strcat(url, "?state=closed");
        } else if (opts->state == GH_ITEM_STATE_ALL) {
            first_param_set ? strcat(url, "?state=all"):
                strcat(url, "?state=all");
        }

        if (opts->filter == GH_ISSUE_FILTER_ALL) {
            first_param_set ? strcat(url, "&filter=all"):
                strcat(url, "?filter=all");
        } else if (opts->filter == GH_ISSUE_FILTER_CREATED) {
            first_param_set ? strcat(url, "?filter=created"):
                strcat(url, "?filter=created");
        } else if (opts->filter == GH_ISSUE_FILTER_MENTIONED) {
            first_param_set ? strcat(url, "?filter=mentioned"):
                strcat(url, "?filter=mentioned");
        } else if (opts->filter == GH_ISSUE_FILTER_SUBSCRIBED) {
            first_param_set ? strcat(url, "?filter=subscribed"):
                strcat(url, "?filter=subscribed");
        } else if (opts->filter == GH_ISSUE_FILTER_REPOS) {
            first_param_set ? strcat(url, "?filter=repos"):
                strcat(url, "?filter=repos");
        }

        if (opts->filter == GH_ISSUE_FILTER_ALL) {
            first_param_set ? strcat(url, "&filter=all"):
                strcat(url, "?filter=all");
        }

        if (opts->order == GH_ORDER_ASC) {
            first_param_set ? strcat(url, "&direction=asc"):
                strcat(url, "?direction=asc");
        }
    }

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_issue_create(const char *owner, const char *repo, const char *data)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/issues");

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);

    CURL_CALL_ERROR_CHECK;
    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_issue_get(const char *owner, const char *repo,
                    const int unsigned id)
{
        gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/issues/");

    char id_val[11] = {0};
    sprintf(id_val, "%d", id);
    strcat(url, id_val);

    SET_BASIC_CURL_CONFIG;

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);
    CURL_CALL_ERROR_CHECK;

    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_issue_update(const char *owner, const char *repo,
                       const int unsigned id, const char *data)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/issues/");

    char id_val[11] = {0};
    sprintf(id_val, "%d", id);
    strcat(url, id_val);

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);

    CURL_CALL_ERROR_CHECK;
    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_issue_lock(const char *owner, const char *repo,
                     const int unsigned id, const char *data)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/issues/");

    char id_val[11] = {0};
    sprintf(id_val, "%d", id);
    strcat(url, id_val);
    strcat(url, "/lock");

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);

    CURL_CALL_ERROR_CHECK;
    CALL_CLEANUP;

    return response;
}

gh_client_response_t*
gh_client_issue_unlock(const char *owner, const char *repo,
                       const int unsigned id)
{
    gh_client_response_t *response = gh_client_response_new();
    struct curl_slist *chunk = NULL;

    char token_header[TOKEN_HEADER_SIZE];
    strcpy(token_header, "Authorization: Bearer ");
    strcat(token_header, token);

    chunk = curl_slist_append(chunk, GH_REQ_JSON_HEADER);
    chunk = curl_slist_append(chunk, token_header);
    chunk = curl_slist_append(chunk, GH_REQ_VER_HEADER);
    chunk = curl_slist_append(chunk, GH_REQ_DEF_UA_HEADER);

    char *url = calloc(DEFAULT_URL_SIZE, sizeof(char));
    strcpy(url, GH_API_REPO_URL);
    strcat(url, owner);
    strcat(url, "/");
    strcat(url, repo);
    strcat(url, "/issues/");

    char id_val[11] = {0};
    sprintf(id_val, "%d", id);
    strcat(url, id_val);
    strcat(url, "/lock");

    SET_BASIC_CURL_CONFIG;
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->resp_code);

    CURL_CALL_ERROR_CHECK;
    CALL_CLEANUP;

    return response;
}

void
gh_client_free()
{
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}