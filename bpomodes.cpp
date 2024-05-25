/*
 *  Boost program-options subcommand handler
 *  RW Penney, May 2024
 */

#include <iostream>
#include "bpomodes.hpp"

namespace BoostPO = boost::program_options;


BpoModes::BpoModes() {
  prepareCommon();
}


BpoModes::BpoModes(const BoostPO::options_description& opts, bool add_help)
  : common_opts(opts) {
  prepareCommon(add_help);
}


BpoModes& BpoModes::add(const std::string& mode,
                        const BoostPO::options_description& opts,
                        ParserInit init) {
  subcommands.emplace(std::make_pair(mode, SubCommand { opts, init }));
  return *this;
}

BoostPO::variables_map BpoModes::parse(int argc, char* argv[]) {
  BoostPO::variables_map varmap;
  bool print_help = false;
  std::string subcommand;

  try {
    //BoostPO::command_line_parser parser(argc, argv);

    BoostPO::positional_options_description podesc;
    podesc.add("subcommand", 1)
          .add("subcmd_args", -1);

    BoostPO::parsed_options parsed =
      BoostPO::command_line_parser(argc, argv)
        .options(common_opts)
        .positional(podesc)
        .allow_unregistered()
        .run();

    BoostPO::store(parsed, varmap);
    print_help = (varmap.count("help") > 0);

    try {
      subcommand = varmap["subcommand"].as<std::string>();
    } catch (std::exception& ex) {
      // Ignore
    }

    if (!print_help) {
      std::vector<std::string> sub_args =
        BoostPO::collect_unrecognized(parsed.options, BoostPO::include_positional);
      sub_args.erase(sub_args.begin());
      handleSub(subcommand, sub_args, varmap);
    }
  } catch (BoostPO::error& ex) {
    std::cerr << argv[0] << ": " << ex.what() << std::endl << std::endl
              << common_opts << std::endl;
    exit(1);
  }

  if (print_help) {
    std::cout << common_opts << std::endl;

    const auto subcmd = subcommands.find(subcommand);
    if (subcmd != subcommands.cend()) {
      std::cout << subcmd->second.opts << std::endl;
    }

    exit(0);
  }

  return varmap;
}


void BpoModes::prepareCommon(bool add_help) {
  if (add_help) {
    common_opts.add_options()
      ("help,h", "Show usage information");
  }

  common_opts.add_options()
    ("subcommand",
      BoostPO::value<std::string>(), "subcommand")
    ("subcmd_args",
      BoostPO::value<std::vector<std::string>>(), "subcommand arguments");
}


void BpoModes::handleSub(const std::string& mode,
                         const std::vector<std::string>& args,
                         BoostPO::variables_map& varmap) const {
  const auto subcmd = subcommands.find(mode);

  if (subcmd == subcommands.cend()) {
    std::stringstream strm;
    bool first = true;

    strm << "subcommand=[";
    for (auto cmd : subcommands) {
      strm << (first ? "" : "|")
           << cmd.first;
      first = false;
    }
    strm << "]";

    throw BoostPO::error(strm.str());
  }

  BoostPO::store(BoostPO::command_line_parser(args)
                    .options(subcmd->second.opts).run(), varmap);
}
