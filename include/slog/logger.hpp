#ifndef SLOG_LOGGER_HPP
# define SLOG_LOGGER_HPP

# include <slog/log_stream.hpp>
# include <slog/tag.hpp>
# include <slog/detail/append_writer_to_logstream.hpp>

# define SLOG(level, ...) slog::logger<__VA_ARGS__>(level, __FILE__, __LINE__)
# define STRACE() SLOG(slog::Level::Debug) << __FILE__ << "(" << __LINE__ << ")"

namespace slog
{
  template <class ... Tag> LogStream logger(Level level, const std::string &file = "", int line = -1)
  {
    LogStream stream(file, line, level, get_prefix<Tag...>());
    if(is_enable<Tag...>())
    {
      detail::append_writer_to_logstream(stream, level);
    }
    return std::move(stream);
  }
}

#endif
