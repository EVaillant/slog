#ifndef SLOG_LOG_STREAM_HPP
# define SLOG_LOG_STREAM_HPP

# include <ostream>

# include <slog/common.hpp>
# include <slog/log_buffer.hpp>

namespace slog
{
  class log_stream
      : public std::ostream
      , private log_buffer
  {
    public:
      log_stream(const char* file, int line, log_level level, bool enable, const mask_tag_type& mask);
      log_stream(log_stream&& m);
  };
}

#endif
