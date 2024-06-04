# $ kshell

A toy shell written in C.

## Getting kshell

1. Clone the repository.
2. Run `make` to build the shell.
3. `./kshell`

Optionally, run `make install` to install `kshell`.

## Features

- Executing commands
- Environment variable expansion (e.g. `$FOO` and `${FOO}`)
- Setting environment variables (e.g. `FOO=bar`)
  - Caveat: quotes are expanded literally, so `FOO=bar` != `FOO="bar"`
- Custom prompt using `$PS1` variable (only literal values, though)
