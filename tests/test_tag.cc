#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <slog/tag.hpp>

BOOST_AUTO_TEST_CASE( tag_enable_01 )
{
  struct my_tag1 : public slog::tag<my_tag1> {};
  struct my_tag2 : public slog::tag<my_tag2> {};

  BOOST_CHECK(slog::is_enable<>());

  BOOST_CHECK(slog::is_enable<my_tag1>());
  BOOST_CHECK(slog::is_enable<my_tag2>());
  BOOST_CHECK((slog::is_enable<my_tag1, my_tag2>()));

  slog::set_enable<my_tag1>(false);

  BOOST_CHECK(!slog::is_enable<my_tag1>());
  BOOST_CHECK(slog::is_enable<my_tag2>());
  BOOST_CHECK((!slog::is_enable<my_tag1, my_tag2>()));
}

BOOST_AUTO_TEST_CASE( tag_enable_02 )
{
  struct my_tag1 : public slog::tag<my_tag1> {};
  struct my_tag2 : public slog::tag<my_tag2> {};
  struct my_tag3 : public slog::tag<my_tag3> {};
  typedef slog::tag_alias<my_tag1, my_tag2> my_tag_alias;

  BOOST_CHECK((slog::is_enable<my_tag3, my_tag_alias>()));
  BOOST_CHECK((slog::is_enable<my_tag_alias, my_tag3>()));

  slog::set_enable<my_tag_alias>(false);

  BOOST_CHECK(!slog::is_enable<my_tag1>());
  BOOST_CHECK(!slog::is_enable<my_tag2>());
  BOOST_CHECK(!slog::is_enable<my_tag_alias>());

  slog::set_enable<my_tag1>(true);

  BOOST_CHECK( slog::is_enable<my_tag1>());
  BOOST_CHECK(!slog::is_enable<my_tag2>());
  BOOST_CHECK(!slog::is_enable<my_tag_alias>());

  slog::set_enable<my_tag2>(true);

  BOOST_CHECK(slog::is_enable<my_tag1>());
  BOOST_CHECK(slog::is_enable<my_tag2>());
  BOOST_CHECK(slog::is_enable<my_tag_alias>());
}

BOOST_AUTO_TEST_CASE( tag_prefix_01 )
{
  struct my_tag1 : public slog::tag<my_tag1> {};
  struct my_tag2 : public slog::tag<my_tag2> {};

  BOOST_CHECK((slog::get_prefix<>() == ""));
  BOOST_CHECK((slog::get_prefix<my_tag1>() == ""));
  BOOST_CHECK((slog::get_prefix<my_tag2>() == ""));
  BOOST_CHECK((slog::get_prefix<my_tag1, my_tag2>() == ""));
  BOOST_CHECK((slog::get_prefix<my_tag2, my_tag1>() == ""));

  slog::set_prefix<my_tag1>("[tyty]");

  BOOST_CHECK((slog::get_prefix<my_tag1>() == "[tyty]"));
  BOOST_CHECK((slog::get_prefix<my_tag1, my_tag2>() == "[tyty]"));

  slog::set_prefix<my_tag2>("[tata]");

  BOOST_CHECK((slog::get_prefix<my_tag2>() == "[tata]"));
  BOOST_CHECK((slog::get_prefix<my_tag1, my_tag2>() == "[tyty]")); // priori to left
  BOOST_CHECK((slog::get_prefix<my_tag2, my_tag1>() == "[tata]")); // priori to left
}

BOOST_AUTO_TEST_CASE( tag_prefix_02 )
{
  struct my_tag1 : public slog::tag<my_tag1> {};
  struct my_tag2 : public slog::tag<my_tag2> {};
  struct my_tag3 : public slog::tag<my_tag3> {};
  typedef slog::tag_alias<my_tag1, my_tag2> my_tag_alias;

  BOOST_CHECK((slog::get_prefix<my_tag3, my_tag_alias>()  == ""));
  BOOST_CHECK((slog::get_prefix<my_tag_alias, my_tag3>()) == "");

  slog::set_prefix<my_tag_alias>("[tutu]");

  BOOST_CHECK(slog::get_prefix<my_tag1>() == "[tutu]");
  BOOST_CHECK(slog::get_prefix<my_tag2>() == "[tutu]");
  BOOST_CHECK(slog::get_prefix<my_tag_alias>() == "[tutu]");

  slog::set_prefix<my_tag2>("[titi]");

  BOOST_CHECK(slog::get_prefix<my_tag1>() == "[tutu]");
  BOOST_CHECK(slog::get_prefix<my_tag2>() == "[titi]");
  BOOST_CHECK(slog::get_prefix<my_tag_alias>() == "[tutu]");

  slog::set_prefix<my_tag1>("[tztz]");

  BOOST_CHECK(slog::get_prefix<my_tag1>() == "[tztz]");
  BOOST_CHECK(slog::get_prefix<my_tag2>() == "[titi]");
  BOOST_CHECK(slog::get_prefix<my_tag_alias>() == "[tztz]");
}


