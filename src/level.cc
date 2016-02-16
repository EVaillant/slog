#include <slog/level.hpp>
#include <cassert>

static const char* str_level[] = {
  "Debug",
  "Verbose",
  "Statistic",
  "Info",
  "Warning",
  "Error",
  "Fatal"
};

std::ostream& operator<<(std::ostream& stream, slog::Level level)
{
  assert(level <= slog::Level::Fatal);

  stream << str_level[(int)level];
  return stream;
}

