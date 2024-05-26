/*
 *  Basic demonstration of Boost program-options with subcommand handling
 *  RW Penney, May 2024
 */

// See also https://gist.github.com/randomphrase/10801888

#include <iostream>
#include "bpomodes.hpp"

namespace BoostPO = boost::program_options;


struct TwoProc: public BpoModes::ModeHandler {
  int run(const BoostPO::variables_map& varmap) {
    std::cerr << "RUNNING TWO:" << std::endl
              << "  logfile: " << varmap["logfile"].as<std::string>() << std::endl
              << "  loglevel: " << varmap["loglevel"].as<int>() << std::endl
              << "  things: " << varmap["things"].as<std::string>() << std::endl;

    return 17;
  }
};


int main(int argc, char* argv[])
{ BoostPO::options_description generic_opts("bpomodes demo"),
                               opts1("mode one"), opts2("mode two"), opts3("mode three");

  generic_opts.add_options()
    ("logfile,L",
      BoostPO::value<std::string>()->default_value("/dev/null"), "location of logfile")
    ("loglevel",
      BoostPO::value<int>()->default_value(1), "logging level");

  BpoModes parser(generic_opts);

  opts1.add_options()
    ("stuff", "do stuff");
  parser.add("one", opts1);

  opts2.add_options()
    ("things", BoostPO::value<std::string>()->default_value("junk"), "get things");
  parser.add("two", opts2, std::make_shared<TwoProc>());

  opts3.add_options()
    ("doobry", "put doobry");
  parser.add("three", opts3);

  const auto vm = parser.parse(argc, argv);

  std::cout << "VARIABLES: " << std::endl;
  for (auto v : vm) {
    std::cout << v.first << "= ???" << std::endl;
  }

  parser.run_subcommand(vm);

  return 0;
}
