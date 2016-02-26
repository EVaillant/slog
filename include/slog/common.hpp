#ifndef SLOG_COMMON_HPP
# define SLOG_COMMON_HPP

# include <chrono>
# include <string>
# include <bitset>
# include <ostream>

namespace slog
{
  static const std::size_t nb_max_tag = 32;

  typedef std::bitset<nb_max_tag> mask_tag_type;
  typedef std::chrono::high_resolution_clock::time_point timestamp;

  enum log_level
  {
    Debug = 0,
    Verbose,
    Statistic,
    Info,
    Warning,
    Error,
    Fatal
  };

  struct entry
  {
    const char*          file;
    int                  line;
    timestamp            time;
    std::string          msg;
    log_level            level;
    const mask_tag_type* mask;
  };

  struct filter
  {
    inline filter()
    {
      reset();
    }

    inline void reset()
    {
      activate = false;
      level    = Debug;
      include.set();
      exclude.reset();
    }

    inline bool accept(log_level l, const mask_tag_type& mask)
    {
      return activate && (l >= level) && ((mask & include) == mask) && !(mask & exclude).any();
    }

    bool          activate;
    log_level     level;
    mask_tag_type include;
    mask_tag_type exclude;
  };

  class logger
  {
    public:
      inline virtual ~logger() 
      {
      }
      
      virtual void write(const entry& e) = 0;
  };
}

std::ostream& operator<<(std::ostream& stream, slog::log_level level);

#endif
