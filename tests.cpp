#include "testdefns.hpp"


namespace bpomodes {
  namespace testing {

/*
 *  ==== TestBare ====
 */

TestBare::TestBare()
  : TestSuite("non-modal operation")
{
  add(BOOST_TEST_CASE(basic));
}


void TestBare::basic()
{ BoostPO::options_description opts;

  opts.add_options()
    ("alpha,a", BoostPO::value<float>(), "alpha-param")
    ("beta", BoostPO::value<double>()->default_value(3.14), "beta-param");

  BpoModes parser(opts);

  { const auto vm = parser.parse("dummy_prog", std::vector<std::string>());

    BOOST_CHECK(vm.find("alpha") == vm.cend());
    BOOST_CHECK_EQUAL(vm["beta"].as<double>(), 3.14);
  }

  { const auto vm = parser.parse("dummy_prog",
                                 split("-a 1.25 --beta 6.28"));
    BOOST_CHECK_EQUAL(vm["alpha"].as<float>(), 1.25f);
    BOOST_CHECK_EQUAL(vm["beta"].as<double>(), 6.28);
  }

  { const auto vm = parser.parse("dummy_prog",
                                 split("--alpha=2.5"));
    BOOST_CHECK_EQUAL(vm["alpha"].as<float>(), 2.5f);
    BOOST_CHECK_EQUAL(vm["beta"].as<double>(), 3.14);
  }
}


/*
 *  ==== TestModes ====
 */

TestModes::TestModes()
  : TestSuite("subcommand modes")
{
  add(BOOST_TEST_CASE(dispatch));
  add(BOOST_TEST_CASE(modename));
}


void TestModes::dispatch() {
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


void TestModes::modename() {
  BoostPO::options_description common_opts, opts_alpha, opts_beta, opts_gamma;

  auto parser = BpoModes(common_opts)
                  .add("alpha", opts_alpha);

  parser.subcommand_param = "chosen_mode";

  parser.add("beta", opts_beta)
        .add("gamma", opts_gamma);

  const std::vector<std::string> modes = { "alpha", "beta", "gamma" };
  for (auto m : modes) {
    const auto vm = parser.parse("dummy_prog", { m });

    BOOST_CHECK_EQUAL(vm["chosen_mode"].as<std::string>(), m);
  }
}


/*
 *  ==== TestModeAPI ====
 */

 TestModeAPI::TestModeAPI()
  : TestSuite("ModeHandler API")
{
  add(BOOST_TEST_CASE(basic));
}


void TestModeAPI::basic() {
  BoostPO::options_description alpha_opts("mode alpha"), beta_opts("mode beta");
  std::shared_ptr hm_alpha = std::make_shared<HM>(),
                  hm_beta = std::make_shared<HM>();

  BpoModes parser;
  parser.subcommand_param = "the_mode";

  alpha_opts.add_options()
    ("a0", BoostPO::value<int>())
    ("a1", BoostPO::value<int>())
    ("a2", BoostPO::value<int>()->default_value(5));
  parser.add("alpha", alpha_opts, hm_alpha);

  beta_opts.add_options()
    ("b0", BoostPO::value<int>())
    ("b1", BoostPO::value<int>()->default_value(17))
    ("b2", BoostPO::value<int>());
  parser.add("beta", beta_opts, hm_beta);

  BOOST_CHECK_EQUAL(hm_alpha->prep_count, 0);
  BOOST_CHECK_EQUAL(hm_beta->prep_count, 0);

  parser.parse("dummy_prog", split("alpha --a0 23"));
  BOOST_CHECK_EQUAL(hm_alpha->prep_count, 1);
  BOOST_CHECK_EQUAL(hm_alpha->ingest_count, 1);
  BOOST_CHECK_EQUAL(hm_alpha->run_count, 0);
  BOOST_CHECK_EQUAL(hm_alpha->vm_keys, "a0|a2|the_mode");

  parser.parse("dummy_prog", split("beta --b1 23"));
  BOOST_CHECK_EQUAL(hm_beta->prep_count, 1);
  BOOST_CHECK_EQUAL(hm_beta->ingest_count, 1);
  BOOST_CHECK_EQUAL(hm_beta->run_count, 0);
  BOOST_CHECK_EQUAL(hm_beta->vm_keys, "b1|the_mode");

  const auto vm = parser.parse("dummy_prog", split("beta --b0 17 --b2 29"));
  const int stat = parser.run_subcommand(vm);
  BOOST_CHECK_EQUAL(hm_beta->prep_count, 2);
  BOOST_CHECK_EQUAL(hm_beta->ingest_count, 2);
  BOOST_CHECK_EQUAL(hm_beta->run_count, 1);
  BOOST_CHECK_EQUAL(hm_beta->vm_keys, "b0|b1|b2|the_mode");
  BOOST_CHECK_EQUAL(stat, 7);
}


  }   // namespace testing
}   // namespace bpomodes
