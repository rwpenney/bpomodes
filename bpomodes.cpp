/*
 *  Boost program-options subcommand handler
 *  RW Penney, May 2024
 */

#include <iostream>
#include "bpomodes.hpp"

namespace BoostPO = boost::program_options;


BpoModes::BpoModes()
  : add_help(true), opts_finalized(false) {}


BpoModes::BpoModes(const BoostPO::options_description& opts, bool add_help)
  : add_help(add_help), opts_finalized(false), common_opts(opts) {}


BpoModes& BpoModes::add(const std::string& mode,
                        const BoostPO::options_description& opts,
                        ModeHandler handler) {
  subcommands.emplace(std::make_pair(mode, SubCommand { opts, handler }));
  return *this;
}

BoostPO::variables_map BpoModes::parse(int argc, char* argv[]) {
  BoostPO::variables_map varmap;
  bool print_help = false;
  std::string subcommand;
  std::map<std::string, SubCommand>::iterator subhandler;

  if (!opts_finalized) finalizeCommon();

  try {
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
      subhandler = subcommands.find(subcommand);

      if (subhandler == subcommands.cend()) {
        throw BoostPO::validation_error(BoostPO::validation_error::invalid_option,
                                        subcommand, "subcommand");
      }

      std::vector<std::string> sub_args =
        BoostPO::collect_unrecognized(parsed.options, BoostPO::include_positional);
      sub_args.erase(sub_args.begin());
      handleSub(subcommand, subhandler->second, sub_args, varmap);
    }
  } catch (BoostPO::error& ex) {
    std::cerr << argv[0] << ": " << ex.what() << std::endl << std::endl
              << common_opts << std::endl;

    if (subhandler != subcommands.cend()) {
      std::cerr << subhandler->second.opts << std::endl;
    }
    exit(1);
  }

  if (print_help) {
    std::cout << common_opts << std::endl;

    if (subhandler != subcommands.cend()) {
      std::cout << subhandler->second.opts << std::endl;
    }

    exit(0);
  }

  return varmap;
}


void BpoModes::finalizeCommon(bool add_help) {
  if (add_help) {
    common_opts.add_options()
      ("help,h", "Show usage information");
  }

  std::stringstream strm;
  bool first = true;

  strm << "subcommand [";
  for (auto cmd : subcommands) {
    strm << (first ? "" : "|")
         << cmd.first;
    first = false;
  }
  strm << "]";

  common_opts.add_options()
    ("subcommand",
      BoostPO::value<std::string>(), strm.str().c_str())
    ("subcmd_args",
      BoostPO::value<std::vector<std::string>>(), "subcommand arguments");

  opts_finalized = true;
}


void BpoModes::handleSub(const std::string& mode, SubCommand& subcmd,
                         const std::vector<std::string>& args,
                         BoostPO::variables_map& varmap) const {
  BoostPO::store(BoostPO::command_line_parser(args)
                    .options(subcmd.opts).run(), varmap);

  subcmd.handler.ingest(varmap);
}

// (C)Copyright 2024, RW Penney
