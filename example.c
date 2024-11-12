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

    // gh_client_response_t *res = gh_client_octocat_says();
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // printf("%ld\n", res->resp_code);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_releases_list("rancher", "rke2", NULL);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // printf("Limit: %d\n", res->rate_limit_data->limit);
    // printf("remaining: %d\n", res->rate_limit_data->remaining);
    // printf("reset: %d\n", res->rate_limit_data->reset);
    // printf("used: %d\n", res->rate_limit_data->used);
    // printf("resource: %s\n", res->rate_limit_data->resource);
    // printf("Next: %s\n", res->next_link);
    // printf("Last: %s\n", res->last_link);
    // printf("Count: %d\n", res->page_count);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_releases_latest("briandowns", "spinner");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_release_by_tag("briandowns", "spinner", "v1.23.1");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_release_by_id("briandowns", "spinner", 160317840);
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

    // gh_client_response_t *res = gh_client_repo_branch_get("briandowns", "spinner", "issue-53");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // const char *data = "{\"new_name\": \"issue-53-2\"}";
    // gh_client_response_t *res = gh_client_repo_branch_rename("briandowns", "spinner", "issue-53", data);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // const char *data = "{\"base\":\"master\",\"head\":\"feature/nothing\",\"commit_message\":\"Shipped cool_feature!\"}";
    // gh_client_response_t *res = gh_client_repo_branch_merge("briandowns", "devops-testing", data);
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

    // gh_client_response_t *res = gh_client_repo_commits_list("briandowns", "devops-testing", NULL);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_pr_commits_list("briandowns", "devops-testing", "508a84e57e22df0247f1e8ccb81298692c0d679a");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_commit_get("briandowns", "spinner", "508a84e57e22df0247f1e8ccb81298692c0d679a");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_commits_compare("briandowns", "spinner", "561dc95eeadf7fc57c2fe6ce2253f0f3361c0f75", "f878506b30a20e7b6c29cd17d93217f5ebd80b0b");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_pull_req_opts_t opts = {
    //     .order = GH_PR_ORDER_ASC,
    // };
    // gh_client_response_t *res = gh_client_repo_pull_request_list("briandowns", "spinner", &opts);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_pull_request_get("briandowns", "spinner", 160, NULL);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_user_logged_in_get();
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_user_by_id_get("galal-hussein");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_user_by_id_hovercard_get("galal-hussein");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_user_blocked_list();
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_user_blocked_by_id("galal-hussein");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%lu\n", res->resp_code);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_user_block_user_by_id("galal-hussein");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%lu\n", res->resp_code);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_user_unblock_user_by_id("galal-hussein");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%lu\n", res->resp_code);
    // gh_client_response_free(res);

    /* always cleanup */
    gh_client_free();
    
    return 0;
}
