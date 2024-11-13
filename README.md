# libgithub

libgithub is a C client library for accessing the [Github REST API](https://docs.github.com/en).

## Usage

Initialize the library. The call requires the user's github token.

```c
gh_client_init(token);
```

Response data is returned in JSON format however the caller is responsible for how to handle the data.

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

## Features

List calls support pagination.

### Repositories
#### Branches

* List branches
* Get a branch
* Rename a branch
* Sync to an upstream
* Merge a branch

#### Commits

* List commits
* Get a commit
* Compare commits
* Get PR from commit

#### Pull Requests

* List pull requests
* Get a pull request by id

#### Release

* List releases
* Create a release
* Generate release notes
* Get latest
* Get by tag
* Get by id
* Update a release
* Delete a release

### Users

* Get info for logged in user
* Get info for user by id
* Get hovercard info
* List blocked users
* Block a user
* Unblock a user

### Rate Limit

Each response from the API includes rate limit data. This information is stored in the `rate_limit_data` field on the response struct.

## Requirements / Dependencies

* libcurl

## Contributing

Please feel free to open a PR!

## Contact

Brian Downs [@bdowns328](http://twitter.com/bdowns328)

## License

BSD 2 Clause [License](/LICENSE).
