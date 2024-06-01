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

  static void basic();
};


/** Test customization points offered via ModeHandler subclasses */
struct TestModeAPI : TestSuite {
  TestModeAPI();

  static void basic();

  struct HM: public BpoModes::ModeHandler {
    unsigned prep_count = 0, ingest_count = 0, run_count = 0;

    std::string vm_keys;

    BoostPO::command_line_parser prepare(BoostPO::command_line_parser& p) {
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
};


  }   // namespace testing
}   // namespace bpomodes
