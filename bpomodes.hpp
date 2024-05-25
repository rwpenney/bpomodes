/*
 *  Boost program-options subcommand handler
 *  RW Penney, May 2024
 */

#pragma once

#include <boost/program_options.hpp>
#include <map>
#include <string>
#include <vector>


class BpoModes {
  public:
    BpoModes();
    BpoModes(const boost::program_options::options_description& common,
             bool add_help=true);

    struct ModeHandler {
      virtual void operator()(boost::program_options::command_line_parser&) {}
      virtual void ingest(const boost::program_options::variables_map&) {}
    };

    BpoModes& add(const std::string& subcmd,
                  const boost::program_options::options_description&,
                  ModeHandler handler=ModeHandler());

    boost::program_options::variables_map parse(int argc, char** argv);

  protected:
    struct SubCommand {
      boost::program_options::options_description opts;
      ModeHandler handler;
    };

    bool add_help;
    bool opts_finalized;
    boost::program_options::options_description common_opts;

    std::map<std::string, SubCommand> subcommands;

    void finalizeCommon(bool add_help=true);
    void handleSub(const std::string& mode, SubCommand& cmd,
                   const std::vector<std::string>& args,
                   boost::program_options::variables_map&) const;
};

// (C)Copyright 2024, RW Penney
