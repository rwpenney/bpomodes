#include "testdefns.hpp"


namespace bpomodes {
  namespace testing {


TestModes::TestModes()
  : TestSuite("subcommand modes")
{
  add(BOOST_TEST_CASE(basic));
}


void TestModes::basic() {
  BoostPO::options_description common_opts("common_options"),
    alpha_opts("mode alpha"), beta_opts("mode beta"), gamma_opts("mode gamma");

  common_opts.add_options()
    ("loglevel,L",
      BoostPO::value<int>()->default_value(0), "logging level");

  BpoModes parser(common_opts);

  alpha_opts.add_options()
    ("stuff", BoostPO::value<std::string>());
  parser.add("alpha", alpha_opts);

  beta_opts.add_options()
    ("things", BoostPO::value<unsigned short>());
  parser.add("beta", beta_opts);

  gamma_opts.add_options()
    ("vacuum", BoostPO::value<double>()->default_value(0.0));
  parser.add("gamma", gamma_opts);

  { const auto vm = parser.parse("dummy_prog",
                                 split("-L 2 alpha --stuff lemon"));

    check_vm_keys(vm, { "subcommand",
                        "loglevel", "stuff" });

    BOOST_CHECK_EQUAL(vm["loglevel"].as<int>(), 2);
    BOOST_CHECK_EQUAL(vm["subcommand"].as<std::string>(), "alpha");
    BOOST_CHECK_EQUAL(vm["stuff"].as<std::string>(), "lemon");
  }

  { const auto vm = parser.parse("dummy_prog",
                                 split("-L 3 beta --things 17"));

    check_vm_keys(vm, { "subcommand",
                        "loglevel", "things" });

    BOOST_CHECK_EQUAL(vm["loglevel"].as<int>(), 3);
    BOOST_CHECK_EQUAL(vm["subcommand"].as<std::string>(), "beta");
    BOOST_CHECK_EQUAL(vm["things"].as<unsigned short>(),17);
  }

  { const auto vm = parser.parse("dummy_prog",
                                 split("-L 5 gamma"));

    check_vm_keys(vm, { "subcommand",
                        "loglevel", "vacuum" });

    BOOST_CHECK_EQUAL(vm["loglevel"].as<int>(), 5);
    BOOST_CHECK_EQUAL(vm["subcommand"].as<std::string>(), "gamma");
    BOOST_CHECK_EQUAL(vm["vacuum"].as<double>(), 0.0);
  }

  { const auto vm = parser.parse("dummy_prog",
                                 split("-L 7 gamma --vacuum 1e-15"));

    check_vm_keys(vm, { "subcommand",
                        "loglevel", "vacuum" });

    BOOST_CHECK_EQUAL(vm["loglevel"].as<int>(), 7);
    BOOST_CHECK_EQUAL(vm["subcommand"].as<std::string>(), "gamma");
    BOOST_CHECK_EQUAL(vm["vacuum"].as<double>(), 1e-15);
  }
}


  }   // namespace testing
}   // namespace bpomodes
