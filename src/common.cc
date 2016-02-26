#include <slog/common.hpp>
#include <cassert>

namespace
{
  static const char* str_level[] = {
    "Debug",
    "Verbose",
    "Statistic",
    "Info",
    "Warning",
    "Error",
    "Fatal"
  };
}

std::ostream& operator<<(std::ostream& stream, slog::log_level level)
{
  assert(level <= slog::Fatal);
  stream << str_level[(int)level];
  return stream;
}
