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

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "../github.h"

void
setUp(void)
{
}

void
tearDown(void)
{
}

void
test_gh_client_set_user_agent(void)
{
    gh_client_set_user_agent("test_user_agent");
}

void
test_gh_client_octocat_says(void)
{
    gh_client_response_t *res = gh_client_octocat_says();

    TEST_ASSERT_NOT_NULL(res);

    gh_client_response_free(res);
}

void
test_gh_client_res_rate_limit(void)
{
    gh_client_response_t *res = gh_client_octocat_says();

    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_NOT_NULL(res->rate_limit_data);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, res->rate_limit_data->limit);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, res->rate_limit_data->remaining);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, res->rate_limit_data->reset);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, res->rate_limit_data->used);

    gh_client_response_free(res);
}

void
test_gh_client_repo_releases_list_nonpaginated(void)
{
    gh_client_response_t *res = gh_client_repo_releases_list("briandowns",
                                                             "spinner", NULL);

    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_INT((int)strlen(res->next_link), 0);

    gh_client_response_free(res);
}

void
test_gh_client_repo_releases_list_paginated(void)
{
    gh_client_response_t *res = gh_client_repo_releases_list("rancher",
                                                             "rke2", NULL);

    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_NOT_NULL(res->next_link);

    gh_client_response_free(res);
}

void
test_gh_client_repo_releases_latest(void)
{
    gh_client_response_t *res = gh_client_repo_releases_latest("briandowns",
                                                               "spinner");

    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_INT(200, res->resp_code);

    gh_client_response_free(res);
}

void
test_gh_client_repo_release_by_id(void)
{
    gh_client_response_t *res = gh_client_repo_release_by_tag("briandowns",
                                                              "spinner",
                                                              "v1.23.1");

    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_INT(200, res->resp_code);

    gh_client_response_free(res);
}

void
test_gh_client_repo_release_assets_list(void)
{
    gh_client_response_t *res = gh_client_repo_release_assets_list(
        "rancher", "rke2", 182819936, NULL);

    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_INT(200, res->resp_code);

    gh_client_response_free(res);
}

void
test_gh_client_repo_release_asset_get(void)
{
    gh_client_response_t *res = gh_client_repo_release_asset_get("rancher",
                                                                 "rke2",
                                                                 203030920);

    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_INT(200, res->resp_code);

    gh_client_response_free(res);
}

void
test_gh_client_repo_commits_list(void)
{
    gh_client_response_t *res = gh_client_repo_commits_list("briandowns", 
                                                            "devops-testing",
                                                            NULL);

    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_INT(200, res->resp_code);

    gh_client_response_free(res);
}

void
test_gh_client_repo_commits_compare(void)
{}

void
test_gh_client_repo_pr_commits_list(void)
{}

void
test_gh_client_repo_branches_list(void)
{}

void
test_gh_client_repo_branch_get(void)
{}

void
test_gh_client_repo_pull_request_list(void)
{}

void
test_gh_client_repo_pull_request_get(void)
{}

void
test_gh_client_user_logged_in_get(void)
{}

void
test_gh_client_user_by_id_get(void)
{}

void
test_gh_client_user_by_id_hovercard_get(void)
{}

void
test_gh_client_user_blocked_list(void)
{}

void
test_gh_client_user_blocked_by_id(void)
{}

void
test_gh_client_user_followers_list(void)
{}

int
main(void)
{
    char *token = getenv("GITHUB_TOKEN");
    if (token == NULL || token[0] == '\0') {
        fprintf(stderr, "github token not set in environment or invalid\n");
        return 1;
    }

    gh_client_init(token);

    UNITY_BEGIN();

    RUN_TEST(test_gh_client_set_user_agent);
    RUN_TEST(test_gh_client_octocat_says);
    RUN_TEST(test_gh_client_res_rate_limit);
    RUN_TEST(test_gh_client_repo_releases_list_nonpaginated);
    RUN_TEST(test_gh_client_repo_releases_list_paginated);
    RUN_TEST(test_gh_client_repo_releases_latest);
    RUN_TEST(test_gh_client_repo_release_by_id);
    RUN_TEST(test_gh_client_repo_release_assets_list);
    RUN_TEST(test_gh_client_repo_release_asset_get);
    RUN_TEST(test_gh_client_repo_commits_list);
    RUN_TEST(test_gh_client_repo_commits_compare);
    RUN_TEST(test_gh_client_repo_pr_commits_list);
    RUN_TEST(test_gh_client_repo_branches_list);
    RUN_TEST(test_gh_client_repo_branch_get);
    RUN_TEST(test_gh_client_repo_pull_request_list);
    RUN_TEST(test_gh_client_repo_pull_request_get);
    RUN_TEST(test_gh_client_user_logged_in_get);
    RUN_TEST(test_gh_client_user_by_id_get);
    RUN_TEST(test_gh_client_user_by_id_hovercard_get);
    RUN_TEST(test_gh_client_user_blocked_list);
    RUN_TEST(test_gh_client_user_blocked_by_id);
    RUN_TEST(test_gh_client_user_followers_list);

    gh_client_free();

    return UNITY_END();
}
