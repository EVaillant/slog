#ifndef SLOG_LEVEL_HPP
# define SLOG_LEVEL_HPP

# include <ostream>

namespace slog
{
  enum class Level
  {
    Debug = 0,
    Verbose,
    Statistic,
    Info,
    Warning,
    Error,
    Fatal
  };
}

std::ostream& operator<<(std::ostream& stream, slog::Level level);

#endif
