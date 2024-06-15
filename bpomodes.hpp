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

    /** The field in the variables_map describing the user-selected subcommand */
    std::string subcommand_param = "subcommand";

    /** Mechanism for handling mode-specific parser setup and extraction */
    struct ModeHandler {
      /** Optionally append lines to --help message */
      virtual void append_help(std::ostream&) {}

      /** Optionally reconfigure the parser before passing command-line arguments */
      virtual boost::program_options::command_line_parser&
      prepare(boost::program_options::command_line_parser& p) {
        return p; }

      /** Optionally update internal state after variables-map has been finalized */
      virtual void ingest(const boost::program_options::variables_map&) {}

      /** Entry-point for main() to delegate to the selected subprogram via BpoModes::run_subcommand() */
      virtual int run(const boost::program_options::variables_map&) {
        return 0; }
    };

    using HandlerSP = std::shared_ptr<ModeHandler>;

    /** Register a subcommand, with associated options and parser customization */
    BpoModes& add(const std::string& subcmd,
                  const boost::program_options::options_description&,
                  HandlerSP handler=nullptr);

    boost::program_options::variables_map parse(int argc, char** argv) {
      return parse((argc > 0 ? argv[0] : ""),
                   boost::program_options::command_line_parser(argc, argv)); }
    boost::program_options::variables_map parse(const std::string& progname,
                                                const std::vector<std::string>& args) {
      return parse(progname, boost::program_options::command_line_parser(args)); }


    int run_subcommand(const boost::program_options::variables_map&);

  protected:
    struct SubCommand {
      boost::program_options::options_description opts;
      HandlerSP handler;
    };

    const std::string subcmd_args_param = "_subcmd_args";

    bool add_help;
    bool opts_finalized;
    boost::program_options::options_description common_opts;
    boost::program_options::options_description hidden_opts;

    using SubCmdMap = std::map<std::string, SubCommand>;
    SubCmdMap subcommands;
    SubCmdMap::iterator selected_subcmd;
    std::string subcommandMenu(const std::string& sep="|") const;

    boost::program_options::variables_map parse(const std::string& progname,
                                                boost::program_options::command_line_parser&&);

    void finalizeCommon(bool add_help=true);
    void handleSub(SubCommand& cmd, const std::vector<std::string>& args,
                   boost::program_options::variables_map&);

    std::ostream& printOpts(std::ostream&);
};

// (C)Copyright 2024, RW Penney
