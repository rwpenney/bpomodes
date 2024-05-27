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


struct TestBare : TestSuite {
  TestBare();

  static void basic();
};


struct TestModes : TestSuite {
  TestModes();

  static void basic();
};


struct TestModeAPI : TestSuite {
  TestModeAPI();

  static void basic();
};


  }   // namespace testing
}   // namespace bpomodes
