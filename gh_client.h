#ifndef __CLIENT_H
#define __CLIENT_H

#define GH_CLIENT_USER_BLOCKED_CODE     204
#define GH_CLIENT_USER_NOT_BLOCKED_CODE 404

/**
 * Default response structure returned for each call
 * to the API. Contains the API response, the response code,
 * response size, any error code, and message if applicable.
 */
typedef struct {
    char *resp;
    char *err_msg;
    size_t size;
    long resp_code;
    int err_code;
} gh_client_response_t;

/**
 * Contains the pull request states to 
 * choose from when listing.
 */
enum gh_pull_request_state {
    GH_PR_STATE_OPENED = 0,
    GH_PR_STATE_CLOSED = 1,
    GH_PR_STATE_MERGED = 2
};

/**
 * Contains the pull request order options
 * when listing.
 */
enum gh_pull_request_order {
    GH_PR_ORDER_DESC = 0,
    GH_PR_ORDER_ASC  = 1
};

/**
 * Structure used to pass additional options
 * when listing pull requests.
 * 
 */
typedef struct {
    enum gh_pull_request_state state;
    enum gh_pull_request_order order;
} gh_client_pull_req_opts_t;

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
 * Retrieve a list of releases for the given repository
 * in JSON format. The response memory needs to be freed by the caller. 
 */
gh_client_response_t*
gh_client_repo_releases_list(const char *owner, const char *repo);

/**
 * Create a new release for the given repository and configuration.
 * Reponse is in JSON format and the memory needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_repo_releases_create(const char *owner, const char *repo,
    const char *data);

/**
 * Retrieve a list of branches for the given repository
 * in JSON format. The response memory needs to be freed by the caller. 
 */
gh_client_response_t*
gh_client_repo_branches_list(const char *owner, const char *repo);

/**
 * Retrieve a list of open pull requests. The response memory
 * needs to be freed by the caller.
 */
gh_client_response_t*
gh_client_repo_pull_request_list(const char *owner, const char *repo,
    gh_client_pull_req_opts_t *opts);

/**
 * Retrieve 1 pull request by id. order option in opts will be
 * ignored. The response memory needs to be freed by the caller. 
 */
gh_client_response_t*
gh_client_repo_pull_request_get(const char *owner, const char *repo,
    const unsigned int id, gh_client_pull_req_opts_t *opts);

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
