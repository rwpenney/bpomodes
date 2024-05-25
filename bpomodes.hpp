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

    struct ParserInit {
      virtual void operator()(boost::program_options::command_line_parser&) {}
      virtual int invoke(const boost::program_options::variables_map&) {
        return 0; }
    };

    BpoModes& add(const std::string& subcmd,
                  const boost::program_options::options_description&,
                  ParserInit init=ParserInit());

    boost::program_options::variables_map parse(int argc, char** argv);

  protected:
    boost::program_options::options_description common_opts;

    struct SubCommand {
      boost::program_options::options_description opts;
      ParserInit init;
    };

    std::map<std::string, SubCommand> subcommands;

    void prepareCommon(bool add_help=true);
    void handleSub(const std::string& mode,
                   const std::vector<std::string>& args,
                   boost::program_options::variables_map&) const;
};
