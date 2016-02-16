#include <slog/tag.hpp>

namespace
{
  static slog::level_writers default_writers;
}

namespace slog
{
  void append_default_writer(Level level, const std::shared_ptr<IEntryWriter>& writer)
  {
    default_writers.push_back(std::make_pair(level, writer));
  }

  void clear_default_writer()
  {
    default_writers.clear();
  }

  const level_writers &get_default_writer()
  {
    return default_writers;
  }
}
