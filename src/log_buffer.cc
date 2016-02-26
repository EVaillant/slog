#include <slog/log_buffer.hpp>
#include <slog/logger_manager.hpp>

#include <utility>
#include <chrono>

namespace slog
{
  log_buffer::log_buffer(const char* file, int line, log_level level, bool enable, const mask_tag_type& mask)
    : enable_(enable)
  {
    ref_entry_.file  = file;
    ref_entry_.line  = line;
    ref_entry_.level = level;
    ref_entry_.mask  = &mask;
    ref_entry_.time  = std::chrono::high_resolution_clock::now();
    current_entry_   = ref_entry_;
  }

  log_buffer::log_buffer(log_buffer&& b)
    : enable_(b.enable_)
    , ref_entry_(std::move(b.ref_entry_))
    , current_entry_(std::move(b.current_entry_))
  {
  }
  
  log_buffer::~log_buffer()
  {
    if(enable_ && !current_entry_.msg.empty())
    {
      push_();
    }
  }

  int log_buffer::overflow(int c)
  {
    if(enable_)
    {
      if(c == '\n')
      {
        push_();
      }
      else
      {
        current_entry_.msg += (char) c;
      }
    }

    return !traits_type::eof();
  }

  void log_buffer::push_()
  {
    entry local_entry = ref_entry_;
    std::swap(current_entry_, local_entry);

    static logger_manager &manager = logger_manager::instance();
    manager.notif_log(std::move(local_entry));
  }
}
