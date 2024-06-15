/*
 *  Unit-test declarations for BpoModes
 */

#pragma once

#include <boost/program_options.hpp>
#include <boost/test/unit_test.hpp>
#include <set>
#include <string>

#include "bpomodes.hpp"

namespace BoostPO = boost::program_options;
namespace BoostUT = boost::unit_test;


namespace bpomodes {
  namespace testing {

struct TestSuite: public BoostUT::test_suite {
  TestSuite(const std::string& ident)
    : BoostUT::test_suite(ident) {}

  static std::vector<std::string> split(const std::string&);
  static void check_vm_keys(const BoostPO::variables_map&,
                            const std::set<std::string>&);
};


/** Test behaviour when no subcommands are defined */
struct TestBare : TestSuite {
  TestBare();

  static void basic();
};


/** Test dispatch of option-handling across subcommands */
struct TestModes : TestSuite {
  TestModes();

  static void dispatch();
  static void modename();
};


/** Test customization points offered via ModeHandler subclasses */
struct TestModeAPI : TestSuite {
  TestModeAPI();

  static void basic();
  static void positional();

  struct MHstats: public BpoModes::ModeHandler {
    unsigned prep_count = 0, ingest_count = 0, run_count = 0;

    std::string vm_keys;

    BoostPO::command_line_parser& prepare(BoostPO::command_line_parser& p) {
      ++prep_count; return p; }
    void ingest(const BoostPO::variables_map& vm) {
      std::stringstream strm;
      bool first = true;
      for (auto kv : vm) {
        strm << (first ? "" : "|") << kv.first;
        first = false; }
      vm_keys = strm.str();
      ++ingest_count; }
    virtual int run(const BoostPO::variables_map& vm) {
      ++run_count; return 7; }
  };

  struct MHpos: public BpoModes::ModeHandler {
    MHpos(const BoostPO::positional_options_description& p): positional(p) {}
    BoostPO::command_line_parser& prepare(BoostPO::command_line_parser& p) {
      p.positional(positional); return p; }
    BoostPO::positional_options_description positional;
  };
};


  }   // namespace testing
}   // namespace bpomodes
