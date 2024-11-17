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

#include <assert.h>
#include <stdio.h>

#include "../github.h"

void
test_gh_client_octocat_says()
{
    gh_client_response_t *res = gh_client_octocat_says();
    assert(res != NULL);
    gh_client_response_free(res);
}

void
test_gh_client_response_free()
{
    gh_client_response_t *res = gh_client_octocat_says();
    assert(res != NULL);
    gh_client_response_free(res);
}

void
test_gh_client_call_unauthorized()
{
    gh_client_response_t *res = gh_client_repo_releases_list("briandowns", "spinner", NULL);
    assert(res != NULL);
    assert(res->resp != NULL);
    assert(res->err_msg == NULL);
    assert(res->err_code == 0);
    assert(res->resp_code = 401);
    gh_client_response_free(res);
}

int
main(int argc, char **argv)
{
    gh_client_init("");
    
    test_gh_client_octocat_says();
    test_gh_client_response_free();
    test_gh_client_call_unauthorized();

    gh_client_free();

    printf("All tests have passed!\n");

    return 0;
}
