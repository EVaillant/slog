#include <slog/basic_entry_writer.hpp>
#include <slog/entry.hpp>

#include <iomanip>
#include <sstream>

#include <unistd.h>
#include <limits.h>

namespace slog
{
  //
  // BasicEntryWriterOStream
  //

  BasicEntryWriterOStream::BasicEntryWriterOStream(std::ostream &stream)
    : stream_(stream)
    , enable_locate_(false)
  {
  }

  void BasicEntryWriterOStream::set_locate(bool value)
  {
    enable_locate_ = value;
  }

  void BasicEntryWriterOStream::write(const std::shared_ptr<Entry> &entry)
  {
    std::ostringstream buffer;

    append_timestamp_(buffer, entry->time);
    buffer << " | ";
    append_level_(buffer, entry->level);
    buffer << " | " << entry->msg;
    append_locate_(buffer, entry->file, entry->line);

    append_stream_(buffer.str());
  }

  void BasicEntryWriterOStream::append_stream_(const std::string &buffer)
  {
    stream_ << buffer << std::endl;
  }

  void BasicEntryWriterOStream::append_timestamp_(std::ostream& stream, const std::chrono::high_resolution_clock::time_point &time)
  {
    std::chrono::high_resolution_clock::duration duration_from_epoch = time.time_since_epoch();
    std::time_t date = std::chrono::duration_cast<std::chrono::seconds>(duration_from_epoch).count();
    int          ms  = std::chrono::duration_cast<std::chrono::milliseconds>(duration_from_epoch).count() - date*1000;

    char tmp[20];
    std::strftime(tmp, 20, "%H:%M:%S", localtime (&date));

    stream << tmp << "." << std::setfill('0') << std::setw(3) << ms;
  }

  void BasicEntryWriterOStream::append_level_(std::ostream &stream, Level level)
  {
    stream << std::left << std::setw(9) << std::setfill(' ') << level;
  }

  void BasicEntryWriterOStream::append_locate_(std::ostream &stream, const std::string &file, int line)
  {
    if(enable_locate_ && !file.empty() && line != -1)
    {
      stream << " (file:" << file << " line:" << line << ")";
    }
  }

  //
  // BasicEntryWriterFStream
  //

  BasicEntryWriterFStream::BasicEntryWriterFStream(const std::string &filename, const std::string &filename_ext)
    : BasicEntryWriterOStream(stream_)
    , filename_ext_(filename_ext)
    , file_counter_(0)
    , nb_line_(0)
    , size_file_(0)
    , max_line_by_file_(-1)
    , max_file_size_(-1)
  {
    std::stringstream stream_filename;
    stream_filename << filename;

    //
    // hostname
    char hostname[HOST_NAME_MAX + 1];
    if(!::gethostname(hostname, HOST_NAME_MAX + 1))
    {
      stream_filename << "_" << hostname;
    }

    //
    // pid
    int pid = ::getpid();
    stream_filename << "_" << pid;

    //
    // date
    std::time_t date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char tmp[20];
    std::strftime(tmp, 20, "%Y%m%d", localtime (&date));
    stream_filename << "_" << tmp;

    filename_ = stream_filename.str();
  }

  void BasicEntryWriterFStream::set_max_line_by_file(int nb)
  {
    if(! stream_.is_open())
    {
      max_line_by_file_ = nb;
    }
  }

  void BasicEntryWriterFStream::set_max_file_size(int size)
  {
    if(! stream_.is_open())
    {
      max_file_size_ = size;
    }
  }

  void BasicEntryWriterFStream::append_stream_(const std::string &buffer)
  {
    int buffer_size = buffer.size();
    if(!stream_.is_open()
       || (max_line_by_file_ != -1 ? (nb_line_ + 1 >= max_line_by_file_) : false)
       || (max_file_size_ != -1 ? (size_file_ + buffer_size >= max_file_size_) : false))
    {
      create_new_file_();
    }

    BasicEntryWriterOStream::append_stream_(buffer);

    // update counter
    if(max_line_by_file_ != -1 || max_file_size_ != -1)
    {
      ++nb_line_;
      size_file_ += buffer_size;
    }
  }

  void BasicEntryWriterFStream::create_new_file_()
  {
    std::stringstream stream;
    stream << filename_;
    if(max_line_by_file_ != -1 || max_file_size_ != -1)
    {
      stream << "_" << std::setfill('0') << std::setw(3) << file_counter_++;
    }
    stream << "." << filename_ext_;

    std::string full_filename = stream.str();
    if(stream_.is_open())
    {
      stream_.close();
    }
    stream_.open(stream.str(), std::fstream::out | std::fstream::trunc);
  }

  //
  // AsyncEntryWriter
  //

  AsyncEntryWriter::AsyncEntryWriter(std::unique_ptr<IEntryWriter> &&writer)
    : writer_(std::move(writer))
    , quit_(false)
  {
    thread_ = std::thread([this]
    ()
      {
        this->async_write_();
      }
    );
  }

  AsyncEntryWriter::~AsyncEntryWriter()
  {
    std::unique_lock<std::mutex> lock(entries_mutex_);
    quit_ = true;
    entries_cond_.notify_all();

    thread_.join();
  }

  void AsyncEntryWriter::write(const std::shared_ptr<Entry> &entry)
  {
    std::unique_lock<std::mutex> lock(entries_mutex_);
    if(!quit_)
    {
      bool need_to_wake_up_job = entries_.empty();
			entries_.push_back(entry);
      if(need_to_wake_up_job)
      {
        entries_cond_.notify_all();
      }
    }
  }

  void AsyncEntryWriter::async_write_()
  {
    std::shared_ptr<Entry> entry;
    while(!quit_ || entry)
    {
      entry = get_first_entry_();
      if(entry)
      {
        writer_->write(entry);
      }
    }
  }

  std::shared_ptr<Entry> AsyncEntryWriter::get_first_entry_()
  {
    std::unique_lock<std::mutex> lock(entries_mutex_);
    while (entries_.empty() && !quit_)
    {
      entries_cond_.wait(lock);
    }

    std::shared_ptr<Entry> ret;
    if(!entries_.empty())
    {
      ret = entries_.front();
      entries_.pop_front();
    }
    return std::move(ret);
  }
}
