#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "github.h"


int
main(void)
{
    char *token = getenv("GITHUB_TOKEN");
    if (token == NULL || token[0] == '\0') {
        fprintf(stderr, "github token not set in environment or invalid\n");
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
    // if (res->resp != NULL) {
    //     printf("%s\n", res->resp);
    // }
    
    // printf("Limit: %" PRIu64 "\n", res->rate_limit_data->limit);
    // printf("remaining: %" PRIu64 "\n", res->rate_limit_data->remaining);
    // printf("reset: %" PRIu64 "\n", res->rate_limit_data->reset);
    // printf("used: %" PRIu64 "\n", res->rate_limit_data->used);
    // printf("resource: %s\n", res->rate_limit_data->resource);

    // printf("Next: %s\n", res->next_link);
    // printf("Last: %s\n", res->last_link);
    // gh_client_response_free(res);

    // gh_client_req_list_opts_t opts = {
    //     .per_page = 100
    // };
    // gh_client_response_t *res = gh_client_repo_releases_list("rancher", "rke2", &opts);
    // if (res->err_msg != NULL) {
    //     fprintf(stderr, "%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // while (res->next_link != NULL) {
    //     gh_client_req_list_opts_t opts = {
    //         .page_url = res->next_link
    //     };
    //     res = gh_client_repo_releases_list("golang", "go", &opts);
    //     if (res->err_msg != NULL) {
    //         fprintf(stderr, "%s\n", res->err_msg);
    //         gh_client_response_free(res);
    //         return 1;
    //     }
    //     printf("%s\n", res->next_link);
    // }

    // gh_client_response_t *res = gh_client_repo_releases_latest("briandowns", "spinner");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_repo_release_by_tag("briandowns", "spinner", "v1.23.1");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_repo_release_by_id("briandowns", "spinner", 160317840);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_repo_branches_list("briandowns", "spinner", NULL);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_repo_branch_get("briandowns", "spinner", "issue-53");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // const char *data = "{\"new_name\": \"issue-53-2\"}";
    // res = gh_client_repo_branch_rename("briandowns", "spinner", "issue-53",
    //                                    data);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // data = "{\"base\":\"master\",\"head\":\"feature/nothing\",
    //        \"commit_message\":\"Shipped cool_feature!\"}";
    // res = gh_client_repo_branch_merge("briandowns", "devops-testing", data);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // data = "{\"tag_name\":\"v0.21.0\",
    //         \"target_commitish\":\"master\",
    //         \"name\":\"v.21.0\",
    //         \"body\":\"Description of the release\",
    //         \"draft\":false,\"prerelease\":false,
    //         \"generate_release_notes\":false}";
    // res = gh_client_repo_release_create("briandowns", "devops-testing", data);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_release_assets_list("rancher", "rke2", 182819936, NULL);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_repo_release_asset_get("rancher", "rke2", 203030920);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_repo_commits_list("briandowns", "devops-testing", NULL);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_repo_pr_commits_list("briandowns", "devops-testing",
    //                                 "508a84e57e22df0247f1e8ccb81298692c0d679a",
    //                                 NULL);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_repo_commit_get("briandowns", "spinner",
    //                             "508a84e57e22df0247f1e8ccb81298692c0d679a");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_repo_commits_compare("briandowns", "spinner",
    //                                "561dc95eeadf7fc57c2fe6ce2253f0f3361c0f75",
    //                                "f878506b30a20e7b6c29cd17d93217f5ebd80b0b");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_pull_req_opts_t pro = {
    //     .order = GH_ORDER_ASC,
    // };
    // res = gh_client_repo_pull_request_list("briandowns", "spinner", &pro);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_repo_pull_request_get("briandowns", "spinner", 160, NULL); 
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

    // res = gh_client_user_by_id_get("galal-hussein");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_user_by_id_hovercard_get("galal-hussein");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_user_blocked_list();
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // res = gh_client_user_blocked_by_id("galal-hussein");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%lu\n", res->resp_code);
    // gh_client_response_free(res);

    // res = gh_client_user_block_by_id("galal-hussein");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%lu\n", res->resp_code);
    // gh_client_response_free(res);

    // res = gh_client_user_unblock_by_id("galal-hussein");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%lu\n", res->resp_code);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_user_followers_list(NULL); 
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_issues_req_opts_t iro = {
    //     .state = GH_ITEM_STATE_OPENED,
    //     .order = GH_ORDER_DESC
    // };
    // res = gh_client_issues_for_user_list(&iro);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // const char *data = "{\"title\": \"Testing from libgithub 2\", 
    //                      \"body\": \"Adding a body to the issue.\",
    //                      \"assignees\":[\"briandowns\"]}";
    // gh_client_response_t *res = gh_client_issue_create("briandowns",
    //                                                    "devops-testing",
    //                                                    data);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_issue_get("briandowns",
    //                                                 "devops-testing", 2);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // const char *data = "{\"labels\": [\"bug\"]}";
    // gh_client_response_t *res = gh_client_issue_update("briandowns",
    //                                                    "devops-testing", 2,
    //                                                    data);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // const char *data = "{\"lock_reason\": \"off-topic\"}";
    // gh_client_response_t *res = gh_client_issue_lock("briandowns",
    //                                                  "devops-testing", 2,
    //                                                  data);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%ld\n", res->resp_code);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_issue_unlock("briandowns",
    //                                                    "devops-testing", 2);
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%ld\n", res->resp_code);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_actions_billing_by_org("rancher"); 
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_codes_of_conduct_list(); 
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_code_of_conduct_get_by_key("citizen_code_of_conduct"); 
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_user_rate_limit_info(); 
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_metrics_community_profile("briandowns", "libgithub");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    // gh_client_response_t *res = gh_client_metrics_repository_clones("briandowns", "libgithub", "day");
    // if (res->err_msg != NULL) {
    //     printf("%s\n", res->err_msg);
    //     gh_client_response_free(res);
    //     return 1;
    // }
    // printf("%s\n", res->resp);
    // gh_client_response_free(res);

    gh_client_response_t *res = gh_client_metrics_page_views("briandowns", "libgithub", "day");
    if (res->err_msg != NULL) {
        printf("%s\n", res->err_msg);
        gh_client_response_free(res);
        return 1;
    }
    printf("%s\n", res->resp);
    gh_client_response_free(res);

    gh_client_free();
    
    return 0;
}
