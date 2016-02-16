#ifndef LOG_ENTRY_HPP
# define LOG_ENTRY_HPP

# include <slog/level.hpp>
# include <chrono>

namespace slog
{
  /**
   * @brief The log Entry
   */
  struct Entry
  {
      /**
       * @brief file origin of log
       *
       * it can be empty.
       */
      std::string file;

      /**
       * @brief line origin of log
       *
       * it can be -1.
       */
      int line;

      /**
       * @brief log's timestamp
       */
      std::chrono::high_resolution_clock::time_point time;

      /**
       * @brief log's message
       */
      std::string msg;

      /**
       * @brief log's level
       */
      Level level;
  };
}

#endif
