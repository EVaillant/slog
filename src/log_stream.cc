#include <slog/log_stream.hpp>
#include <utility>

namespace slog
{
  log_stream::log_stream(const char* file, int line, log_level level, bool enable, const mask_tag_type& mask)
    : std::ostream(this)
    , log_buffer(file, line, level, enable, mask)
  {
  }

  log_stream::log_stream(log_stream&& m)
    : std::ostream(this)
    , log_buffer(std::move(m))
  {
  }
}

