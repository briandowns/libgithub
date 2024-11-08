#ifndef __CLIENT_H
#define __CLIENT_H

/**
 * @brief 
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
 * @brief 
 * 
 */
enum gh_pull_request_state {
    OPEN = 0,
    CLOSED = 1
};

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
gh_client_repo_releases_create(const char *owner, const char *repo, const char *data);

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
 * @brief Retrieve a list of open pull requests
 * 
 * @param owner 
 * @param repo 
 * @return gh_client_response_t* 
 */
gh_client_response_t*
gh_client_repo_pull_request_list(const char *owner, const char *repo, enum gh_pull_request_state);

/**
 * @brief Free the memory used by the client.
 * 
 */
void
gh_client_free();

#endif /** end __CLIENT_H */
