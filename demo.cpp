/*
 *  Basic demonstration of Boost program-options with subcommand handling
 *  RW Penney, May 2024
 */

#include <iostream>
#include "bpomodes.hpp"

namespace BoostPO = boost::program_options;


struct TwoProc: public BpoModes::ModeHandler {
  int run(const BoostPO::variables_map& varmap) {
    std::cerr << "RUNNING SUBCOMMAND TWO:" << std::endl
              << "  logfile: " << varmap["logfile"].as<std::string>() << std::endl
              << "  loglevel: " << varmap["loglevel"].as<int>() << std::endl
              << "  things: " << varmap["things"].as<std::string>() << std::endl;

    return 17;
  }
};


struct ThreeProc: public BpoModes::ModeHandler {
  unsigned counter = 0;

  void append_help(std::ostream& strm) {
    strm << "Count in integer steps" << std::endl;
  }

  void ingest(const BoostPO::variables_map& varmap) {
    counter = varmap["counter"].as<unsigned>();
  }

  int run(const BoostPO::variables_map& varmap) {
    std::cerr << "RUNNING SUBCOMMAND THREE:" << std::endl;
    for (unsigned i=0; i<counter; ++i) std::cerr << i << ",";
    std::cerr << std::endl;

    return 0;
  }
};


int main(int argc, char* argv[])
{ BoostPO::options_description
    generic_opts("bpomodes demo"),
    opts1("mode one"),
    opts2("mode two"),
    opts3("mode three");

  // Define options that will be usable across all subcommands
  generic_opts.add_options()
    ("logfile,L",
      BoostPO::value<std::string>()->default_value("/dev/null"),
      "location of logfile")
    ("loglevel",
      BoostPO::value<int>()->default_value(1), "logging level");

  // Create the parser with the common options_description
  BpoModes parser(generic_opts);

  // Define the options that will be usable in mode "one", and add them to the parser
  opts1.add_options()
    ("stuff", "do stuff");
  parser.add("one", opts1);

  // Define the options that will be usable in mode "one",
  // and add them to the parser together with a ModeHandler
  opts2.add_options()
    ("things", BoostPO::value<std::string>()->default_value("junk"), "get things");
  parser.add("two", opts2, std::make_shared<TwoProc>());

  // Define the options that will be usable in mode "three", and add them to the parser
  opts3.add_options()
    ("counter,c",
     BoostPO::value<unsigned>()->default_value(0),
     "how many things to count");
  parser.add("three", opts3, std::make_shared<ThreeProc>());

  // Parse the supplied command-line arguments,
  // extracting values into a variables_map:
  const auto vm = parser.parse(argc, argv);

  std::cout << "Selected subcommand: "
      << vm["subcommand"].as<std::string>() << std::endl
      << "VARIABLES: " << std::endl;
  for (auto v : vm) {
    std::cout << v.first << "= ???" << std::endl;
  }

  // Delegate further processing to any of the available ModeHandlers:
  parser.run_subcommand(vm);

  return 0;
}
