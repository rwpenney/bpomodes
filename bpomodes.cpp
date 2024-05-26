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
                        HandlerSP handler) {
  if (!handler) {
    handler.reset(new ModeHandler);
  }

  subcommands.emplace(std::make_pair(mode, SubCommand { opts, handler }));
  selected_subcmd = subcommands.end();

  return *this;
}


/** Digest the command-line arguments
 *
 *  This operates in two phases, first handling the shared options,
 *  and then passing runhandled arguments to a second parser
 *  customized for the selected subcommand.
 */
BoostPO::variables_map BpoModes::parse(const std::string& progname,
                                       BoostPO::command_line_parser&& parser) {
  BoostPO::variables_map varmap;
  std::string subcommand;
  bool print_help = false;

  if (!opts_finalized) finalizeCommon();
  selected_subcmd = subcommands.end();

  try {
    BoostPO::positional_options_description podesc;
    podesc.add("subcommand", 1)
          .add("subcmd_args", -1);

    BoostPO::parsed_options parsed_opts =
      parser.options(common_opts)
            .positional(podesc)
            .allow_unregistered()
            .run();

    BoostPO::store(parsed_opts, varmap);
    print_help = (varmap.count("help") > 0);

    try {
      subcommand = varmap["subcommand"].as<std::string>();
      selected_subcmd = subcommands.find(subcommand);
    } catch (std::exception& ex) {
      // Postpone handling unresolved subcommands
    }

    if (!print_help) {
      if (selected_subcmd == subcommands.cend()) {
        throw BoostPO::validation_error(BoostPO::validation_error::invalid_option,
                                        subcommand, "subcommand");
      }

      std::vector<std::string> sub_args =
        BoostPO::collect_unrecognized(parsed_opts.options,
                                      BoostPO::include_positional);
      sub_args.erase(sub_args.begin());

      handleSub(selected_subcmd->second, sub_args, varmap);
    }
  } catch (BoostPO::error& ex) {
    std::cerr << progname << ": " << ex.what() << std::endl << std::endl;
    printOpts(std::cerr);
    exit(1);
  }

  if (print_help) {
    printOpts(std::cout);
    exit(0);
  }

  return varmap;
}


int BpoModes::run_subcommand(const BoostPO::variables_map& varmap) {
  if (selected_subcmd == subcommands.end()
      || !selected_subcmd->second.handler) {
    throw BoostPO::validation_error(BoostPO::validation_error::invalid_option,
                                    "nullptr", "subcommand");
  }

  return selected_subcmd->second.handler->run(varmap);
}


/** Finalize the shared options_description once all subcommands are known */
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


void BpoModes::handleSub(SubCommand& subcmd,
                         const std::vector<std::string>& args,
                         BoostPO::variables_map& varmap) {
  HandlerSP selected_handler = subcmd.handler;

  auto parser =
    BoostPO::command_line_parser(args)
      .options(subcmd.opts);

  BoostPO::store(selected_handler->prepare(parser).run(), varmap);
  selected_handler->ingest(varmap);
}


std::ostream& BpoModes::printOpts(std::ostream& strm) {
  strm << common_opts << std::endl;

  if (selected_subcmd != subcommands.cend()) {
    strm << selected_subcmd->second.opts << std::endl;
  }

  return strm;
}

// (C)Copyright 2024, RW Penney
