# Boost program-options subcommand parser

This is a simple wrapper for
[boost::program_options](https://www.boost.org/doc/libs/1_85_0/doc/html/program_options.html)
which allows basic handling of "subcommands" where a single C++ application
provides multiple operating modes which each have their own configuration options.
It is analogous to the `add_subparsers()` functionality of
[Python's argparse](https://docs.python.org/3/library/argparse.html) module.

The supplied demo application illustrates basic usage of the library
and handles command-line combinations such as:

    ./bpo-demo --help
    ./bpo-demo -h one
    ./bpo-demo --loglevel 17 two
    ./bpo-demo --logfile some_path.txt three --counter 31

The `-h` or `--help` options are automatically inserted into the common
options, and generate mode-specific messages such as:

    bpomodes demo:
      -L [ --logfile ] arg (=/dev/null) location of logfile
      --loglevel arg (=1)               logging level
      -h [ --help ]                     Show usage information
      [one|three|two]
      <subcommand_args> ...

    mode three:
      -c [ --counter ] arg (=0) how many things to count

    Count in integer steps

`BpoModes` was originally influenced by
[A.Rankine's GitHub gist](https://gist.github.com/randomphrase/10801888).


## Usage

The library consists of a single C++11 class, `BpoModes`, which manages
a collection of `boost::program_options::options_description` objects,
one for each subcommand, and another that represent options shared
by all subcommands. In its simplest form, one simply populates
each of these `options_description` objects and passes them
to `BpoModes::add()`, and then calls the `parse()` method to
generate a `variables_map`:

    using namespace boost::program_options;
    options_description generic_options, options_one, options_two;

    generic_options.add_options()
        /* add generic and subcommand options */ ;

    BpoModes parser(generic_options);

    parser.add("subcommand_one", options_one);
    parser.add("subcommand_two", options_two);

    const auto varmap = parser.parse(argc, argv);

The field `varmap["subcommand"].as<std::string>()` will be populated
with the name of the subcommand as passed to the `BpoModes::add()` method.

More sophisticated use-cases can use the third, optional, argument
to `BpoModes::add()`, which should be a subclass of `BpoModes::ModeHandler`.
That interface provides three customization points for setting up
the underlying `command_line_parser` before it sees the vector
of command-line arguments; or for providing an entry-point that can
be called from `main()` to delegate processing of the subcommand.

The [demo.cpp](demo.cpp) file shows more detail about how these components
fit together. Running `./bpo-demo --help` will show information
about the available subcommands.
