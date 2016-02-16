#include <slog/log_buffer.hpp>

#include <slog/entry.hpp>
#include <slog/ientry_writer.hpp>

#include <chrono>

namespace slog
{
  LogBuffer::LogBuffer(const std::string &file, int line, Level level, const std::string &prefix)
    : file_(file)
    , line_ (line)
    , level_ (level)
    , prefix_(prefix)
  {
  }

  LogBuffer::LogBuffer(LogBuffer &&l)
    : file_(std::move(l.file_))
    , line_(l.line_)
    , level_(l.level_)
    , prefix_(std::move(l.prefix_))
    , entry_(std::move(l.entry_))
    , writers_(std::move(l.writers_))
  {
  }

  LogBuffer::~LogBuffer()
  {
    push_();
  }

  void LogBuffer::append(const std::shared_ptr<IEntryWriter> &writer)
  {
    writers_.insert(writer);
  }

  void LogBuffer::create_entry_()
  {
    entry_ = std::make_shared<Entry>();
    entry_->file  = file_;
    entry_->line  = line_;
    entry_->level = level_;
    entry_->msg   = prefix_ + " ";
    entry_->time  = std::chrono::high_resolution_clock::now();
  }

  int LogBuffer::overflow(int c)
  {
    if(!writers_.empty())
    {
      if(!entry_)
      {
        create_entry_();
      }

      if(c == '\n')
      {
        push_();
      }
      else
      {
        entry_->msg += (char) c;
      }
    }

    return !traits_type::eof();
  }

  void LogBuffer::push_()
  {
    if(entry_)
    {
      for(auto &writer : writers_)
      {
        writer->write(entry_);
      }

      entry_.reset();
    }
  }
}
