/*
 *  Unit-test drivers for BpoModes
 */

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>
#include "testdefns.hpp"


namespace bpomodes {
  namespace testing {


std::vector<std::string> TestSuite::split(const std::string& line) {
  std::vector<std::string> args;

  boost::split(args, line, boost::is_any_of(" \t"),
               boost::algorithm::token_compress_on);

  return args;
}


void TestSuite::check_vm_keys(const BoostPO::variables_map& vm,
                              const std::set<std::string>& expected) {
  std::set<std::string> observed;

  for (auto kv : vm) observed.insert(kv.first);

  BOOST_CHECK_EQUAL_COLLECTIONS(observed.cbegin(), observed.cend(),
                                expected.cbegin(), expected.cend());
}


struct RootSuite : TestSuite {
  RootSuite()
    : TestSuite("bpomodes tests")
  {
    add(BOOST_TEST_CASE(splitting));

    add(new TestModes);
    add(new TestModeAPI);
  }

  static void splitting() {
    const auto result = split("alpha beta   gamma \t \t delta");
    const std::vector<std::string> expected { "alpha", "beta", "gamma", "delta" };

    BOOST_CHECK_EQUAL_COLLECTIONS(result.cbegin(), result.cend(),
                                  expected.cbegin(), expected.cend());
  }
};


//! Unit-test initialization mechanism for boost::unit_test
bool init_unit_test_suite()
{
  BoostUT::framework::master_test_suite().add(new RootSuite);
  return true;
}


  }   // namespace testing
}   // namespace rtimers


int main(int argc, char *argv[])
{
  return BoostUT::unit_test_main(
          (BoostUT::init_unit_test_func)bpomodes::testing::init_unit_test_suite,
          argc, argv);
}
