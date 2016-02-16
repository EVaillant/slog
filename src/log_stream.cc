#include <slog/log_stream.hpp>

namespace slog
{
  LogStream::LogStream(const std::string &file, int line, Level level, const std::string &prefix)
    : std::ostream(this)
    , LogBuffer(file, line, level, prefix)
  {
  }

  LogStream::LogStream(LogStream &&m)
    : std::ostream(this)
    , LogBuffer(std::move(m))
  {
  }

  void LogStream::append_writer(const std::shared_ptr<IEntryWriter> &writer)
  {
    this -> append(writer);
  }
}

