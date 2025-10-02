# libgithub

[![Build and Deploy](https://github.com/briandowns/libgithub/actions/workflows/main.yml/badge.svg)](https://github.com/briandowns/libgithub/actions/workflows/compile_example.yml/badge.svg)

libgithub is a C client library for accessing the [Github REST API](https://docs.github.com/en) and has been tested on Linux (Debian variants), MacOS, and FreeBSD with GCC and Clang, amd64 and arm64 architectures.

## Usage

To initialize the library, the user's github token is required.

```c
gh_client_init(token);
```

Clean up after use.
```c
gh_client_free();
```

### Error Handling

If an error occurs during the call, the error will be put into the `response->err_msg` field and `response->resp` will be set to `NULL`.

```c
gh_client_response_t *res = gh_client_repo_release_by_tag("briandowns", "spinner", "v1.29.16");
if (res->err_msg != NULL) {
    fprintf(stderr, "%s\n", res->err_msg);
    gh_client_response_free(res);
}
```

## Build shared object

To build the shared object:

```sh
make install
```

## Example 

Build the example:

```sh
make example
```

## API Coverage

* API response data is returned in a string containing JSON.
* The caller is responsible for how to handle the data.
* List calls support pagination.
* Each response from the API includes rate limit data.


### Repositories

- [x] List repositories
- [x] Retrieve a repository
- [x] Create repository
- [x] Update repository
- [x] Delete repository
- [x] List languages in repository

#### Branches

- [x] List branches
- [x] Get a branch
- [x] Rename a branch
- [x] Sync to an upstream
- [x] Merge a branch

#### Commits

- [x] List commits
- [x] Get a commit
- [x] Compare commits
- [x] Get PR from commit

#### Pull Requests

- [x] List pull requests
- [x] Get a pull request by id
- [ ] Create a pull request
- [ ] Update a pull request
- [ ] List pull request files
- [ ] Check if pull request has been merged
- [ ] Merge a pull request
- [ ] Update a pull request branch

#### Release

- [x] List releases
- [x] Create a release
- [x] Generate release notes
- [x] Get latest
- [x] Get by tag
- [x] Get by id
- [x] Update a release
- [x] Delete a release

##### Assets

- [x] List release assets
- [x] Get a release asset
- [ ] Update a release asset
- [ ] Delete a release asset
- [ ] Upload a release asset

#### Stargazers

- [x] List stargazers

### Issues

- [x] List issues assigned to logged in user
- [x] List issues by repository
- [x] Create an issue
- [x] Get by id
- [x] Update an issue
- [x] Lock issue
- [x] Unlock issue

### Users

- [x] Get info for logged in user
- [x] Get info for user by id
- [x] Get hovercard info
- [x] List blocked users
- [x] Block a user
- [x] Unblock a user
- [x] List followers
- [x] Get rate limit info for auth'd user
- [x] List starred repositories

### Metrics

- [x] Community Profile Metrics
- [x] Repository Clones data
- [x] Top 10 referral paths
- [x] Top referral sources
- [x] Number of page views

### Activity

- [x] List events by organization
- [x] List events by user

### Billing

- [x] Get action billing by org

### Code of Conduct 

- [x] Get all codes of conduct
- [x] Get a code of conduct by key

## Requirements / Dependencies

* libcurl

## Contributing

Please feel free to open a PR!

## Contact

Brian Downs [@bdowns328](http://twitter.com/bdowns328)

## License

BSD 2 Clause [License](/LICENSE).
