#ifndef SLOG_SLOG_HPP
# define SLOG_SLOG_HPP

# include <slog/log_stream.hpp>
# include <slog/tag.hpp>

# define SLOG(level, ...) slog::make_stream<__VA_ARGS__>(level, __FILE__, __LINE__)
# define STRACE() SLOG(slog::Debug) << __FILE__ << "(" << __LINE__ << ")"

namespace slog
{
  template <class ... Tags> log_stream make_stream(log_level level, const char* file = nullptr, int line = -1)
  {
    typedef tag_alias<Tags...> tags_type;
    static mask_tag_type mask = make_mask_tag<tags_type>();
    return log_stream(file, line, level, is_enable<tags_type>(), mask);
  }
}

#endif
