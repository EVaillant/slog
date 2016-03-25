#ifndef SLOG_LOG_BUFFER_HPP
# define SLOG_LOG_BUFFER_HPP

# include <streambuf>
# include <sstream>
# include <memory>
# include <set>

# include <slog/common.hpp>

namespace slog
{
  class log_buffer : public std::streambuf
  {
    public:
      log_buffer(log_buffer&& b);
      log_buffer(const char* file, int line, log_level level, bool enable, const mask_tag_type& mask);
      virtual ~log_buffer();

    protected:
      void push_();
      entry make_entry_();

      // from std::streambuf
      virtual int overflow(int c);

    private:
      bool  enable_;
      entry ref_entry_;
      entry current_entry_;
  };
}

#endif
