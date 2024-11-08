#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "gh_client.h"

// curl --request GET
// --url "https://api.github.com/octocat"
// --header "Authorization: Bearer YOUR-TOKEN"
// --header "X-GitHub-Api-Version: 2022-11-28"

int
main(int argc, char **argv)
{
    char *token = getenv("GITHUB_TOKEN");
    if (token == NULL) {
        fprintf(stderr, "github token not set in environment\n");
        return 1;
    }

    gh_client_init(token);

    // gh_client_response_t *res = gh_client_octocat_says();
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_releases_list("briandowns", "spinner");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_branches_list("briandowns", "spinner");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // const char *data = "{\"tag_name\":\"v0.21.0\",\"target_commitish\":\"master\",\"name\":\"v.21.0\",\"body\":\"Description of the release\",\"draft\":false,\"prerelease\":false,\"generate_release_notes\":false}";
    // gh_client_response_t *res = gh_client_repo_releases_create("briandowns", "devops-testing", data);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    gh_client_response_t *res = gh_client_repo_pull_request_list("briandowns", "spinner");
    if (res->err_msg != NULL) {
        printf("%s\n", res->err_msg);
        gh_client_response_free(res);
        return 1;
    }
    printf("%s\n", res->resp);
    gh_client_response_free(res);

    /* always cleanup */
    gh_client_free();
    
    return 0;
}
