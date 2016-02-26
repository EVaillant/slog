#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <slog/basic_logger.hpp>
#include <slog/logger_manager.hpp>
#include <slog/slog.hpp>

#include <chrono>
#include <memory>
#include <list>
#include <iostream>

namespace
{
  static const slog::timestamp default_time = std::chrono::high_resolution_clock::from_time_t(1366838651LL) + std::chrono::milliseconds(25);

  class override_time_logger : public slog::logger
  {
    public:
      override_time_logger(std::unique_ptr<slog::logger>&& logger, const slog::timestamp& time)
        : logger_(std::forward<std::unique_ptr<slog::logger>>(logger))
        , time_(time)
      {
      }

      virtual void write(const slog::entry& entry) override
      {
        slog::entry l_entry = entry;
        l_entry.time = time_;
        logger_->write(l_entry);
      }
    private:
      std::unique_ptr<slog::logger> logger_;
      slog::timestamp               time_;
  };

  class auto_clean
  {
    public:
      auto_clean()
      {
      }

      ~auto_clean()
      {
        for(auto id : ids_)
        {
          slog::logger_manager::instance().remove_logger(id);
        }
      }

      void operator +=(slog::logger_manager::logger_id_type id)
      {
        ids_.push_back(id);     
      }
    private:
      std::list<slog::logger_manager::logger_id_type> ids_;
  };

  static slog::logger_manager::logger_id_type create_writer(std::ostream& stream)
  {
    std::unique_ptr<slog::basic_ostream_logger> stream_logger(new slog::basic_ostream_logger(stream));
    std::unique_ptr<override_time_logger>     override_logger(new override_time_logger(std::move(stream_logger), default_time));
    return slog::logger_manager::instance().append_logger(override_logger.release(), true);
  }
}

BOOST_AUTO_TEST_CASE( basic_entry_writer_ostream_01 )
{
  slog::entry entry;
  entry.file  = "ee.cc";
  entry.line  = 66;
  entry.level = slog::Warning;
  entry.msg   = "pouet truc";
  entry.time  = default_time;

  std::stringstream stream;
  slog::basic_ostream_logger writer(stream);

  writer.write(entry);
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Warning   | pouet truc\n");
  stream.str("");

  entry.level = slog::Statistic;
  writer.write(entry);
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Statistic | pouet truc\n");
  stream.str("");

  writer.set_locate(true);

  writer.write(entry);
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Statistic | pouet truc (file:ee.cc line:66)\n");
  stream.str("");
}

BOOST_AUTO_TEST_CASE(logger_01)
{
  auto_clean auto_clean_logger;

  std::stringstream stream;
  auto_clean_logger += create_writer(stream);

  SLOG(slog::Debug) << "bla bla";
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Debug     | bla bla\n");
  stream.str("");

  SLOG(slog::Debug) << "bla bla" << std::endl;
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Debug     | bla bla\n");
  stream.str("");

  {
    auto log_stream = SLOG(slog::Fatal);
    log_stream << "ah ah ah" << std::endl;
    log_stream << "bh bh bh" << std::endl;
  }
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Fatal     | ah ah ah\n23:24:11.025 | Fatal     | bh bh bh\n");
  stream.str("");

  {
    auto log_stream = SLOG(slog::Error);
    log_stream << "ah ah ah\nbh bh bh";
  }
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Error     | ah ah ah\n23:24:11.025 | Error     | bh bh bh\n");
  stream.str("");
}

BOOST_AUTO_TEST_CASE(logger_02)
{
  auto_clean auto_clean_logger;

  std::stringstream stream1;
  slog::logger_manager::logger_id_type id1 = create_writer(stream1);
  auto_clean_logger += id1;
  slog::logger_manager::instance().get_filter(id1).level = slog::Debug;

  std::stringstream stream2;
  slog::logger_manager::logger_id_type id2 = create_writer(stream2);
  auto_clean_logger += id2;
  slog::logger_manager::instance().get_filter(id2).level = slog::Error;

  SLOG(slog::Debug) << "bla bla";
  BOOST_CHECK_EQUAL(stream1.str(), "23:24:11.025 | Debug     | bla bla\n");
  BOOST_CHECK_EQUAL(stream2.str(), "");
  stream1.str("");
  stream2.str("");

  SLOG(slog::Fatal) << "bla bla";
  BOOST_CHECK_EQUAL(stream1.str(), "23:24:11.025 | Fatal     | bla bla\n");
  BOOST_CHECK_EQUAL(stream2.str(), "23:24:11.025 | Fatal     | bla bla\n");
  stream1.str("");
  stream2.str("");
}
