#ifndef __CLIENT_H
#define __CLIENT_H

#define GH_CLIENT_USER_BLOCKED_CODE     204
#define GH_CLIENT_USER_NOT_BLOCKED_CODE 404

#define GH_API_BASE_URL  "https://api.github.com"
#define GH_API_REPO_URL  GH_API_BASE_URL "/repos/"
#define GH_API_USER_URL  GH_API_BASE_URL "/user"
#define GH_API_USERS_URL GH_API_BASE_URL "/users/"

/**
 * Contains the rate limit information returned from each API call.
 */
typedef struct {
    int limit;
    int remaining;
    int reset;
    int used;
} gh_client_rate_limit_data_t;

/**
 * Default response structure returned for each call to the API. Contains the
 * API response, the response code, response size, any error code, and message
 * if applicable.
 */
typedef struct {
    char *resp;
    char *err_msg;
    size_t size;
    long resp_code;
    int err_code;
    gh_client_rate_limit_data_t *rate_limit_data;
} gh_client_response_t;

/**
 * Contains the pull request states to choose from when listing.
 */
enum gh_pull_request_state {
    GH_PR_STATE_OPENED = 0,
    GH_PR_STATE_CLOSED = 1,
    GH_PR_STATE_MERGED = 2
};

/**
 * Contains the pull request order options when listing.
 */
enum gh_pull_request_order {
    GH_PR_ORDER_DESC = 0,
    GH_PR_ORDER_ASC  = 1
};

/**
 * Structure used to pass additional options when listing pull requests.
 */
typedef struct {
    enum gh_pull_request_state state;
    enum gh_pull_request_order order;
} gh_client_pull_req_opts_t;

/**
 * Structure used to pass pagination settings.
 */
typedef struct {
    unsigned int page;
    unsigned int per_page;
} gh_client_req_list_opts_t;

/**
 * Structure used to pass additional options when listing commits.
 */
typedef struct {
    char *sha;
    char *path;
    char *author;
    char *committer;
    char *since;           // expected format: YYYY-MM-DDTHH:MM:SSZ
    char *until;           // expected format: YYYY-MM-DDTHH:MM:SSZ
    unsigned int per_page; // default: 30
    unsigned int page;     // default: 1
} gh_client_commits_list_opts_t;

/**
 * Initialize the library.
 */
int
gh_client_init(const char *token);

/**
 * Free the memory used in the client response.
 */
void
gh_client_response_free(gh_client_response_t *res);

/**
 * Retrieve an octocat response giving
 * an interesitng saying. The response memory
 * needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_octocat_says();

/**
 * Retrieve a list of releases for the given repository. The response memory
 * needs to be freed by the caller. 
 */
gh_client_response_t*
gh_client_repo_releases_list(const char *owner, const char *repo,
                             const gh_client_req_list_opts_t *opts);

/**
 * Retrieve the latest release for the given repository. The response memory
 * needs to be freed by the caller. 
 */
gh_client_response_t*
gh_client_repo_releases_latest(const char *owner, const char *repo);

/**
 * Retrieve a release by the given tag. The response memory needs to be freed
 * by the caller. 
 */
gh_client_response_t*
gh_client_repo_release_by_tag(const char *owner, const char *repo,
                              const char *tag);

/**
 * Retrieve a release by the given id. The response memory needs to be freed by
 * the caller. 
 */
gh_client_response_t*
gh_client_repo_release_by_id(const char *owner, const char *repo,
                             const unsigned int id);

/**
 * Create a new release for the given repository and configuration.
 * The response memory needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_repo_releases_create(const char *owner, const char *repo,
                               const char *data);

/**
 * Update a release for the given repository and configuration. The response
 * memory needs to be freed by the caller.
 * 
 * data argument must be JSON in the following format:
 * 
 * {"tag_name":"v1.0.0","target_commitish":"master","name":"v1.0.0",
 *  "body":"Description of the release","draft":false,"prerelease":false}
 */
gh_client_response_t*
gh_client_repo_releases_update(const char *owner, const char *repo,
                               const unsigned int id, const char *data);

/**
 * Delete a release for the given repository and configuration. The response
 * memory needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_repo_releases_delete(const char *owner, const char *repo,
                               const unsigned int id);

/**
 * Generate release notes content for a release. The response memory needs to
 * be freed by the caller.
 * 
 * data argument must be JSON in the following format:
 * 
 * tag_name (required)
 * 
 * {"tag_name":"v1.0.0","target_commitish":"main","previous_tag_name":"v0.9.2",
 *  "configuration_file_path":".github/custom_release_config.yml"}
 */
gh_client_response_t*
gh_client_repo_releases_gen_notes(const char *owner, const char *repo,
                                  const char *data);

/**
 * Retrieve commits for a given repository. The response memory needs to be
 * freed by the caller.
 */
gh_client_response_t*
gh_client_repo_commits_list(const char *owner, const char *repo,
                            const gh_client_commits_list_opts_t *opts);

/**
 * Retrieve a single commit. The response memory needs to be
 * freed by the caller.
 */
gh_client_response_t*
gh_client_repo_commit_get(const char *owner, const char *repo,
                          const char *sha);

/**
 * Compare 2 commits. The response memory needs to be
 * freed by the caller.
 * 
 * The format of hte 
 */
gh_client_response_t*
gh_client_repo_commits_compare(const char *owner, const char *repo,
                               const char *base, const char *head);

/**
 * Retrieve the merged pull request that introduced the commit. The response
 * memory needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_repo_pr_commits_list(const char *owner, const char *repo,
                               const char *sha);

/**
 * Retrieve a list of branches for the given repository
 * in JSON format. The response memory needs to be freed by the caller. 
 */
gh_client_response_t*
gh_client_repo_branches_list(const char *owner, const char *repo);

/**
 * Retrieve the given branch. The response memory needs to be freed by the
 * caller. 
 */
gh_client_response_t*
gh_client_repo_branch_get(const char *owner, const char *repo,
                          const char *branch);

/**
 * Rename the given branch. The response memory needs to be freed by the
 * caller. 
 * 
 * data argument must be JSON in the following format:
 * '{"new_name":"my_renamed_branch"}'
 */
gh_client_response_t*
gh_client_repo_branch_rename(const char *owner, const char *repo,
                             const char *branch, const char *data);

/**
 * Sync the given branch in a fork to the given upstream. The response memory
 * needs to be freed by the caller. 
 * 
 * data argument must be JSON in the following format:
 * '{"branch":"<branch-name>"}'
 */
gh_client_response_t*
gh_client_repo_branch_sync_upstream(const char *owner, const char *repo,
                                    const char *branch, const char *data);

/**
 * Merge a branch. The response memory needs to be freed by the caller. 
 * 
 * data argument must be JSON in the following format:
 * '{"base":"master","head":"cool_feature",
 *   "commit_message":"Shipped cool_feature!"}'
 */
gh_client_response_t*
gh_client_repo_branch_merge(const char *owner, const char *repo,
                            const char *data);

/**
 * Retrieve a list of open pull requests. The response memory
 * needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_repo_pull_request_list(const char *owner, const char *repo,
                                 const gh_client_pull_req_opts_t *opts);

/**
 * Retrieve 1 pull request by id. order option in opts will be
 * ignored. The response memory needs to be freed by the caller. 
 */
gh_client_response_t*
gh_client_repo_pull_request_get(const char *owner, const char *repo,
                                const unsigned int id,
                                const gh_client_pull_req_opts_t *opts);

/**
 * Retrieve account information for the user currently logged in. 
 * The response memory needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_user_logged_in_get();

/**
 * Retrieve account information for the given username. The response
 * memory needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_user_by_id_get(const char *username);

/**
 * Retrieve hovercard for the given username. The response memory
 * needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_user_by_id_hovercard_get(const char *username);

/**
 * Retrieve a list of blocked users for the currently logged in user.
 * The response memory needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_user_blocked_list();

/**
 * Checks if the given username is blocked by the currenty logged in
 * user. If the response code is 204, the given user is blocked but 
 * if the response code is 404, the given user is not blocked. The
 * response memory needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_user_blocked_by_id(const char *username);

/**
 * Blocks a user by the given id. The response memory needs to be freed
 * by the caller.
 */
gh_client_response_t*
gh_client_user_block_user_by_id(const char *username);

/**
 * Unblocks a user by the given id. The response memory needs to be freed
 * by the caller.
 */
gh_client_response_t*
gh_client_user_unblock_user_by_id(const char *username);

/**
 * Free the memory used by the client.
 */
void
gh_client_free();

#endif /** end __CLIENT_H */
