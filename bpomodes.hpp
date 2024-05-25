/*
 *  Boost program-options subcommand handler
 *  RW Penney, May 2024
 */

#pragma once

#include <boost/program_options.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>


/** Mechanism for parsing command-line options with program submodes
 *
 *  This allows handling of git-like recipes such as
 *  ./my_prog --common-option mode --mode-option
 */
class BpoModes {
  public:
    BpoModes();
    BpoModes(const boost::program_options::options_description& common,
             bool add_help=true);

    /** Mechanism for handling mode-specific parser setup and extraction */
    struct ModeHandler {
      /** Optionally reconfigure the parser before passing command-line arguments */
      virtual boost::program_options::command_line_parser
      prepare(boost::program_options::command_line_parser& p) {
        return p; }

      /** Optionally update internal state after variables-map has been finalized */
      virtual void ingest(const boost::program_options::variables_map&) {}

      /** Application entry-point for handling the selected subprogram */
      virtual int run(const boost::program_options::variables_map&) {
        return 0; }
    };

    using HandlerSP = std::shared_ptr<ModeHandler>;

    /** Register a subcommand, with associated options and parser customization */
    BpoModes& add(const std::string& subcmd,
                  const boost::program_options::options_description&,
                  HandlerSP handler=nullptr);

    boost::program_options::variables_map parse(int argc, char** argv);
    int run_subcommand(const boost::program_options::variables_map&);

  protected:
    struct SubCommand {
      boost::program_options::options_description opts;
      HandlerSP handler;
    };

    bool add_help;
    bool opts_finalized;
    boost::program_options::options_description common_opts;

    HandlerSP selected_handler;

    std::map<std::string, SubCommand> subcommands;

    void finalizeCommon(bool add_help=true);
    void handleSub(const std::string& mode, SubCommand& cmd,
                   const std::vector<std::string>& args,
                   boost::program_options::variables_map&);
};

// (C)Copyright 2024, RW Penney
