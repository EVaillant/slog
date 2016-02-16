#ifndef SLOG_LOGGER_STREAM_HPP
# define SLOG_LOGGER_STREAM_HPP

# include <ostream>
# include <slog/log_buffer.hpp>

namespace slog
{
  class LogStream
      : public std::ostream
      , private LogBuffer
  {
    public:
      LogStream(const std::string& file, int line, Level level, const std::string &prefix);
      LogStream(LogStream &&m);

      void append_writer(const std::shared_ptr<IEntryWriter> &writer);
  };
}

#endif
