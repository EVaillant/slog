#ifndef SLOG_LOG_DEVICE_HPP
# define SLOG_LOG_DEVICE_HPP

# include <streambuf>
# include <sstream>
# include <memory>
# include <set>

# include <slog/level.hpp>

namespace slog
{
  struct Entry;
  class IEntryWriter;

  class LogBuffer : public std::streambuf
  {
    public:
      LogBuffer(const std::string& file, int line, Level level, const std::string &prefix);
      LogBuffer(LogBuffer && l);

      virtual ~LogBuffer();

      void append(const std::shared_ptr<IEntryWriter> &writer);

    protected:
      void create_entry_();
      void push_();

      // from std::streambuf
      virtual int overflow(int c);

    private:
      std::string file_;
      int line_;
      Level level_;
      std::string prefix_;

      std::shared_ptr<Entry> entry_;
      std::set<std::shared_ptr<IEntryWriter>> writers_;
  };
}

#endif
