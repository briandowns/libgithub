#ifndef __CLIENT_H
#define __CLIENT_H

/**
 * @brief Default response structure returned for each call
 * to the API. Contains the API response, the response code,
 * response size, any error code, and message if applicable.
 * 
 */
typedef struct {
    char *resp;
    char *err_msg;
    size_t size;
    int resp_code;
    int err_code;
} gh_client_response_t;

/**
 * @brief Contains the pull request states to 
 * choose from when listing.
 * 
 */
enum gh_pull_request_state {
    GH_PR_STATE_OPENED = 0,
    GH_PR_STATE_CLOSED = 1,
    GH_PR_STATE_MERGED = 2
};

/**
 * @brief Contains the pull request order options
 * when listing.
 * 
 */
enum gh_pull_request_order {
    GH_PR_ORDER_DESC = 0,
    GH_PR_ORDER_ASC  = 1
};

/**
 * @brief Structure used to pass additional options
 * when listing pull requests.
 * 
 */
typedef struct {
    enum gh_pull_request_state state;
    enum gh_pull_request_order order;
} gh_client_pull_req_opts_t;

/**
 * @brief Initialize the library.
 * 
 * @param token 
 * @return int 
 */
int
gh_client_init(const char *token);

/**
 * @brief Free the memory used in the client response.
 * 
 * @param res 
 */
void
gh_client_response_free(gh_client_response_t *res);

/**
 * @brief Retrieve an octocat response giving
 * an interesitng saying. The response memory
 * needs to be freed by the caller.
 * 
 * @return gh_client_response_t* 
 */
gh_client_response_t*
gh_client_octocat_says();

/**
 * @brief Retrieve a list of releases for the given repository
 * in JSON format. The response memory needs to be freed by the caller.
 * 
 * @param owner 
 * @param repo 
 * @return gh_client_response_t* 
 */
gh_client_response_t*
gh_client_repo_releases_list(const char *owner, const char *repo);

/**
 * @brief Create a new release for the given repository and configuration.
 * Reponse is in JSON format and the memory needs to be freed by the caller.
 * 
 * @param owner 
 * @param repo 
 * @param data 
 * @return gh_client_response_t* 
 */
gh_client_response_t*
gh_client_repo_releases_create(const char *owner, const char *repo,
    const char *data);

/**
 * @brief Retrieve a list of branches for the given repository
 * in JSON format. The response memory needs to be freed by the caller.
 * 
 * @param owner 
 * @param repo 
 * @return gh_client_response_t* 
 */
gh_client_response_t*
gh_client_repo_branches_list(const char *owner, const char *repo);

/**
 * @brief Retrieve a list of open pull requests. The response memory
 * needs to be freed by the caller.
 * 
 * @param owner 
 * @param repo 
 * @return gh_client_response_t* 
 */
gh_client_response_t*
gh_client_repo_pull_request_list(const char *owner, const char *repo,
    gh_client_pull_req_opts_t *opts);

/**
 * @brief Retrieve 1 pull request by id. order option in opts will be
 * ignored. The response memory needs to be freed by the caller.
 * 
 * @param owner
 * @param repo
 * @param id
 * @return gh_client_response_t* 
 */
gh_client_response_t*
gh_client_repo_pull_request_get(const char *owner, const char *repo,
    const unsigned int id, gh_client_pull_req_opts_t *opts);

/**
 * @brief Retrieve account information for the user currently logged in. 
 * The response memory needs to be freed by the caller.
 * 
 */
gh_client_response_t*
gh_client_user_logged_in_get();

/**
 * @brief Retrieve account information for the given username. The response
 * memory needs to be freed by the caller.
 * 
 */
gh_client_response_t*
gh_client_user_by_id_get(const char *username);

/**
 * @brief Retrieve hovercard for the given username. The response memory
 * needs to be freed by the caller.
 * 
 */
gh_client_response_t*
gh_client_user_by_id_hovercard_get(const char *username);

/**
 * @brief Free the memory used by the client.
 * 
 */
void
gh_client_free();

#endif /** end __CLIENT_H */
