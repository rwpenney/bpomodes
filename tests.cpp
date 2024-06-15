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
  add(BOOST_TEST_CASE(positional));
}


void TestModeAPI::basic() {
  BoostPO::options_description alpha_opts("mode alpha"), beta_opts("mode beta");
  std::shared_ptr mh_alpha = std::make_shared<MHstats>(),
                  mh_beta = std::make_shared<MHstats>();

  BpoModes parser;
  parser.subcommand_param = "the_mode";

  alpha_opts.add_options()
    ("a0", BoostPO::value<int>())
    ("a1", BoostPO::value<int>())
    ("a2", BoostPO::value<int>()->default_value(5));
  parser.add("alpha", alpha_opts, mh_alpha);

  beta_opts.add_options()
    ("b0", BoostPO::value<int>())
    ("b1", BoostPO::value<int>()->default_value(17))
    ("b2", BoostPO::value<int>());
  parser.add("beta", beta_opts, mh_beta);

  BOOST_CHECK_EQUAL(mh_alpha->prep_count, 0);
  BOOST_CHECK_EQUAL(mh_beta->prep_count, 0);

  parser.parse("dummy_prog", split("alpha --a0 23"));
  BOOST_CHECK_EQUAL(mh_alpha->prep_count, 1);
  BOOST_CHECK_EQUAL(mh_alpha->ingest_count, 1);
  BOOST_CHECK_EQUAL(mh_alpha->run_count, 0);
  BOOST_CHECK_EQUAL(mh_alpha->vm_keys, "a0|a2|the_mode");

  parser.parse("dummy_prog", split("beta --b1 23"));
  BOOST_CHECK_EQUAL(mh_beta->prep_count, 1);
  BOOST_CHECK_EQUAL(mh_beta->ingest_count, 1);
  BOOST_CHECK_EQUAL(mh_beta->run_count, 0);
  BOOST_CHECK_EQUAL(mh_beta->vm_keys, "b1|the_mode");

  const auto vm = parser.parse("dummy_prog", split("beta --b0 17 --b2 29"));
  const int stat = parser.run_subcommand(vm);
  BOOST_CHECK_EQUAL(mh_beta->prep_count, 2);
  BOOST_CHECK_EQUAL(mh_beta->ingest_count, 2);
  BOOST_CHECK_EQUAL(mh_beta->run_count, 1);
  BOOST_CHECK_EQUAL(mh_beta->vm_keys, "b0|b1|b2|the_mode");
  BOOST_CHECK_EQUAL(stat, 7);
}


void TestModeAPI::positional() {

  BpoModes parser;

  BoostPO::options_description alpha_opts("mode_alpha");
  alpha_opts.add_options()
    ("count,c", BoostPO::value<int>())
    ("input", BoostPO::value<std::string>())
    ("output", BoostPO::value<std::string>());
  BoostPO::positional_options_description alpha_pos;
  alpha_pos.add("input", 1);
  alpha_pos.add("output", 1);
  parser.add("alpha", alpha_opts, std::make_shared<MHpos>(alpha_pos));

  BoostPO::options_description beta_opts("mode_beta");
  beta_opts.add_options()
    ("value,x", BoostPO::value<double>()->default_value(-2.0))
    ("source", BoostPO::value<std::string>())
    ("outputs", BoostPO::value<std::vector<std::string>>());
  BoostPO::positional_options_description beta_pos;
  beta_pos.add("source", 1);
  beta_pos.add("outputs", -1);
  parser.add("beta", beta_opts, std::make_shared<MHpos>(beta_pos));

  { const auto vm = parser.parse("dummy_prog",
                                 split("alpha the_input the_output"));
    BOOST_CHECK_EQUAL(vm["subcommand"].as<std::string>(), "alpha");
    BOOST_CHECK_EQUAL(vm["input"].as<std::string>(), "the_input");
    BOOST_CHECK_EQUAL(vm["output"].as<std::string>(), "the_output");
  }

  { const auto vm = parser.parse("dummy_prog",
                                 split("alpha -c 17 --input the_input --output the_output"));
    BOOST_CHECK_EQUAL(vm["subcommand"].as<std::string>(), "alpha");
    BOOST_CHECK_EQUAL(vm["count"].as<int>(), 17);
    BOOST_CHECK_EQUAL(vm["input"].as<std::string>(), "the_input");
    BOOST_CHECK_EQUAL(vm["output"].as<std::string>(), "the_output");
  }

  { const auto vm = parser.parse("dummy_prog",
                                 split("beta the_source output0"));
    BOOST_CHECK_EQUAL(vm["subcommand"].as<std::string>(), "beta");
    BOOST_CHECK_EQUAL(vm["value"].as<double>(), -2.0);
    BOOST_CHECK_EQUAL(vm["source"].as<std::string>(), "the_source");
    BOOST_CHECK_EQUAL(vm["outputs"].as<std::vector<std::string>>().front(), "output0");
  }

  { const auto vm = parser.parse("dummy_prog",
                                 split("beta -x 23.0 the_source output0 output1 output2"));
    BOOST_CHECK_EQUAL(vm["subcommand"].as<std::string>(), "beta");
    BOOST_CHECK_EQUAL(vm["value"].as<double>(), 23.0);
    BOOST_CHECK_EQUAL(vm["source"].as<std::string>(), "the_source");

    const auto outs = vm["outputs"].as<std::vector<std::string>>();
    const auto expected = std::vector<std::string> { "output0", "output1", "output2" };
    BOOST_CHECK_EQUAL_COLLECTIONS(outs.cbegin(), outs.cend(),
                                  expected.cbegin(), expected.cend());
  }
}


  }   // namespace testing
}   // namespace bpomodes
