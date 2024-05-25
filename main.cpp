/*
 *  Boost program-options with subcommand handling
 *  RW Penney, May 2024
 */

// See also https://gist.github.com/randomphrase/10801888

#include <iostream>
#include "bpomodes.hpp"

namespace BoostPO = boost::program_options;


int main(int argc, char* argv[])
{

  BoostPO::options_description generic_opts("bpomodes demo"),
                               opts1("mode one"), opts2("mode two"), opts3("mode three");

  generic_opts.add_options()
    ("logfile,L", "location of logfile")
    ("loglevel", "logging level");

  BpoModes parser(generic_opts);

  opts1.add_options()
    ("stuff", "do stuff");
  parser.add("one", opts1);

  opts2.add_options()
    ("things", BoostPO::value<std::string>()->default_value("junk"), "get things");
  parser.add("two", opts2);

  opts3.add_options()
    ("doobry", "put doobry");
  parser.add("three", opts3);
  
  const auto vm = parser.parse(argc, argv);

  std::cout << "VARIABLES: " << std::endl;
  for (auto v : vm) {
    std::cout << v.first << "= ???" << std::endl;
  }

  return 0;
}
