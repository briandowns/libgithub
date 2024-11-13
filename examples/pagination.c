#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "../gh_client.h"


int
main(int argc, char **argv)
{
    char *token = getenv("GITHUB_TOKEN");
    if (token == NULL) {
        fprintf(stderr, "github token not set in environment\n");
        return 1;
    }

    gh_client_init(token);

    gh_client_req_list_opts_t opts = {
            .per_page = 100
        };
    gh_client_response_t *res = gh_client_repo_releases_list("rancher", "rke2", &opts);
    if (res->err_msg != NULL) {
        fprintf(stderr, "%s\n", res->err_msg);
        gh_client_response_free(res);
        return 1;
    }

    while (res->next_link != NULL) {
        gh_client_req_list_opts_t opts = {
            .per_page = 1,
            .page_url = res->next_link
        };
        res = gh_client_repo_releases_list("golang", "go", &opts);
        if (res->err_msg != NULL) {
            fprintf(stderr, "%s\n", res->err_msg);
            gh_client_response_free(res);
            return 1;
        }
        printf("%s\n", res->next_link);
    }

    gh_client_response_free(res);

    gh_client_free();
    
    return 0;
}
