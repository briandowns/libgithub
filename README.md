# libgithub

libgithub is a C library used to interact with the Github REST API. Response data is returned in JSON format however the caller is responsible for how to handle the data.

## Usage

Initialize the library. The call requires the user's github token.

```c
gh_client_init(token);
```

## Example 

Build the example:

```sh
make example
```

## Build shared object

To build the shared object:

```sh
make install
```

## Install shared object and header

```sh
make install
```

## Features

### Repositories
#### Commits

* List commits
* Get a commit
* Compare commits
* Get PR from commit

#### Branches

* List branches
* Get a branch
* Rename a branch
* Sync to an upstream
* Merge a branch

#### Release

* List releases
* Create a release
* Generate release notes
* Get latest
* Get by tag
* GEt by id

#### Pull Requests

* List pull requests
* Get a pull request by id

### Users

* Get info for logged in user
* Get info for user by id
* List blocked users
* Block a user
* Unblock a user

## Requirements / Dependencies

* libcurl

## Contact

Brian Downs [@bdowns328](http://twitter.com/bdowns328)

## License

BSD 2 Clause [License](/LICENSE).
