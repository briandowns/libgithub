#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "gh_client.h"


int
main(int argc, char **argv)
{
    char *token = getenv("GITHUB_TOKEN");
    if (token == NULL) {
        fprintf(stderr, "github token not set in environment\n");
        return 1;
    }

    gh_client_init(token);

    gh_client_issues_req_opts_t iro = {
        .state = GH_ITEM_STATE_OPENED,
        .order = GH_ORDER_DESC
    };
    gh_client_response_t *res = gh_client_issues_for_user_list(&iro);
    if (res->err_msg != NULL) {
        printf("%s\n", res->err_msg);
        gh_client_response_free(res);
        return 1;
    }
    printf("%s\n", res->resp);
    printf("%ld\n", res->resp_code);
    gh_client_response_free(res);

    gh_client_free();
    
    return 0;
}
