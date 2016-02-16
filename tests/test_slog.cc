#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <slog/basic_entry_writer.hpp>
#include <slog/entry.hpp>
#include <slog/logger.hpp>

#include <chrono>

namespace
{
  std::chrono::high_resolution_clock::time_point default_time = std::chrono::high_resolution_clock::from_time_t(1366838651LL) + std::chrono::milliseconds(25);

  class OverrideTime : public slog::IEntryWriter
  {
    public:
      OverrideTime(const std::shared_ptr<slog::IEntryWriter> &writer, const std::chrono::high_resolution_clock::time_point &time)
        : writer_(writer)
        , time_(time)
      {
      }

      virtual void write(const std::shared_ptr<slog::Entry>& entry) override
      {
        entry->time = time_;
        writer_->write(entry);
      }

    private:
      std::shared_ptr<slog::IEntryWriter> writer_;
      std::chrono::high_resolution_clock::time_point time_;
  };

  template <class ... Tag> class AutoClean
  {
    public:
      AutoClean()
      {
      }

      ~AutoClean()
      {
        slog::clear_writter<Tag...>();
        slog::clear_default_writer();
      }
  };

  static std::shared_ptr<slog::IEntryWriter> create_writer(std::ostream& stream)
  {
    std::shared_ptr<slog::BasicEntryWriterOStream> stream_writer  = std::make_shared<slog::BasicEntryWriterOStream>(stream);
    std::shared_ptr<OverrideTime>                 override_writer = std::make_shared<OverrideTime>(stream_writer, default_time);

    return override_writer;
  }
}

BOOST_AUTO_TEST_CASE( basic_entry_writer_ostream_01 )
{
  std::shared_ptr<slog::Entry> entry = std::make_shared<slog::Entry>();
  entry->file  = "ee.cc";
  entry->line  = 66;
  entry->level = slog::Level::Warning;
  entry->msg   = "pouet truc";
  entry->time  = default_time;

  std::stringstream stream;
  slog::BasicEntryWriterOStream writer(stream);

  writer.write(entry);
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Warning   | pouet truc\n");
  stream.str("");

  entry->level = slog::Level::Statistic;
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
  AutoClean<> auto_clean_writter;

  std::stringstream stream;
  std::shared_ptr<slog::IEntryWriter> writer = create_writer(stream);
  slog::append_default_writer(slog::Level::Debug, writer);

  SLOG(slog::Level::Debug) << "bla bla";
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Debug     |  bla bla\n");
  stream.str("");

  SLOG(slog::Level::Debug) << "bla bla" << std::endl;
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Debug     |  bla bla\n");
  stream.str("");

  {
    auto log_stream = SLOG(slog::Level::Fatal);
    log_stream << "ah ah ah" << std::endl;
    log_stream << "bh bh bh" << std::endl;
  }
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Fatal     |  ah ah ah\n23:24:11.025 | Fatal     |  bh bh bh\n");
  stream.str("");

  {
    auto log_stream = SLOG(slog::Level::Error);
    log_stream << "ah ah ah\nbh bh bh";
  }
  BOOST_CHECK_EQUAL(stream.str(), "23:24:11.025 | Error     |  ah ah ah\n23:24:11.025 | Error     |  bh bh bh\n");
  stream.str("");
}

BOOST_AUTO_TEST_CASE(logger_02)
{
  AutoClean<> auto_clean_writter;

  std::stringstream stream1;
  std::shared_ptr<slog::IEntryWriter> writer1 = create_writer(stream1);
  slog::append_default_writer(slog::Level::Debug, writer1);

  std::stringstream stream2;
  std::shared_ptr<slog::IEntryWriter> writer2 = create_writer(stream2);
  slog::append_default_writer(slog::Level::Error, writer2);

  SLOG(slog::Level::Debug) << "bla bla";
  BOOST_CHECK_EQUAL(stream1.str(), "23:24:11.025 | Debug     |  bla bla\n");
  BOOST_CHECK_EQUAL(stream2.str(), "");
  stream1.str("");
  stream2.str("");

  SLOG(slog::Level::Fatal) << "bla bla";
  BOOST_CHECK_EQUAL(stream1.str(), "23:24:11.025 | Fatal     |  bla bla\n");
  BOOST_CHECK_EQUAL(stream2.str(), "23:24:11.025 | Fatal     |  bla bla\n");
  stream1.str("");
  stream2.str("");
}

