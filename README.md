# CRUCIBLE commandline tool

A tool to create [Crucible](https://de.atlassian.com/software/crucible) code reviews based on your last commit.

It´s basically a port of [this gist](https://gist.github.com/evant/8937831) to the [Reason](https://reasonml.github.io/) language and some additional features.

## Prerequisites

In oder to build this tool, please install [Reason](https://reasonml.github.io/).

Only tested in **MacOS**

## Build

- clone this repo
- `cd cru`
- Make sure opam is installed: `brew install gpatch opam`
- Init opam: `opam init`
- Install dune `opam install dune`
- Install dependencies `opam install .`
- run `make build` to build the project
- this creates a new executable at `/_build/install/default/bin/reason-native-bin`
- link the executable `ln -s $PWD/_build/install/default/bin/reason-native-bin /usr/local/bin/cru` to make it available via your commandline

## Configuration

This tool expects a `.review.json` file in your home directory. This file holds your credentials and a configuration for each local directory that is linked to a Crucible repo.

- Run `cp .review.example.json ~/.review.json`
- Open `~/.review.json` in your editor and adjust the settings
- You can add as many projects as you want

```json
{
  "crucible": {
    "url": "https://url-to-your-crucible-api/review",
    "userName": "your.username",
    "password": "4ny!pa$$"
  },
  "projects": [
    {
      "path": "/Users/me/code/myproject",
      "key": "KEY-OF-PROJECT",
      "repo": "repo_name"
    }
  ]
}
```

## Usage

Type `cru --help` to see a list of supported commands

### `cru create` or just `cru`

The default command creates a new review and adds a new changeset to review based on your last commit. In order to make this work, this commit has to be pushed to the remote already.

### `cru add`

Adds an additional changeset to the last created review.

### `cru history`

Shows all created reviews. All created reviews are saved in `~/.review.history.json`.
