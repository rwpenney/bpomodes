# Boost program-options subcommand parser

This is a simple wrapper for
[boost::program_options](https://www.boost.org/doc/libs/1_85_0/doc/html/program_options.html)
which allows basic handling of "subcommands" where a single C++ application
provides multiple operating modes which each have their own configuration options.
It is loosely analogous to the `add_subparsers()` functionality of
[Python's argparse](https://docs.python.org/3/library/argparse.html) module,
although far less polished.

The supplied demo application illustrates basic usage of the library
and handles command-line combinations such as:

    ./bpo-demo --help
    ./bpo-demo -h one
    ./bpo-demo --loglevel 17 two
    ./bpo-demo --logfile some_path.txt three --doobry

This library was originally influenced by
[A.Rankine's GitHub gist](https://gist.github.com/randomphrase/10801888)
