#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <slog/tag.hpp>

BOOST_AUTO_TEST_CASE( tag_enable_01 )
{
  struct my_tag : public slog::tag<my_tag> {};
  BOOST_CHECK(slog::is_enable<my_tag>());
  slog::set_enable<my_tag>(false);
  BOOST_CHECK(!slog::is_enable<my_tag>());
}

BOOST_AUTO_TEST_CASE( tag_enable_02 )
{
  struct my_tag1 : public slog::tag<my_tag1> {};
  struct my_tag2 : public slog::tag<my_tag2> {};
  typedef slog::tag_alias<my_tag1, my_tag2> my_tag_alias;

  BOOST_CHECK(slog::is_enable<my_tag1>());
  BOOST_CHECK(slog::is_enable<my_tag2>());
  BOOST_CHECK(slog::is_enable<my_tag_alias>());

  slog::set_enable<my_tag2>(false);

  BOOST_CHECK(slog::is_enable<my_tag1>());
  BOOST_CHECK(!slog::is_enable<my_tag2>());
  BOOST_CHECK(!slog::is_enable<my_tag_alias>());

  slog::set_enable<my_tag_alias>(true);

  BOOST_CHECK(slog::is_enable<my_tag1>());
  BOOST_CHECK(slog::is_enable<my_tag2>());
  BOOST_CHECK(slog::is_enable<my_tag_alias>());

  slog::set_enable<my_tag_alias>(false);

  BOOST_CHECK(!slog::is_enable<my_tag1>());
  BOOST_CHECK(!slog::is_enable<my_tag2>());
  BOOST_CHECK(!slog::is_enable<my_tag_alias>());
}

BOOST_AUTO_TEST_CASE( tag_enable_03 )
{
  typedef slog::tag_alias<> my_tag_alias;

  BOOST_CHECK(slog::is_enable<my_tag_alias>());
  slog::set_enable<my_tag_alias>(false);
  BOOST_CHECK(slog::is_enable<my_tag_alias>());
}
