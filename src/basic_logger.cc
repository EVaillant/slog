#include <slog/basic_logger.hpp>

#include <iomanip>
#include <sstream>

#include <unistd.h>
#include <limits.h>

namespace slog
{
  //
  // basic_ostream_logger
  //
  basic_ostream_logger::basic_ostream_logger(std::ostream &stream)
    : stream_(stream)
    , enable_locate_(false)
  {
  }

  void basic_ostream_logger::set_locate(bool value)
  {
    enable_locate_ = value;
  }

  void basic_ostream_logger::write(const entry& e)
  {
    std::ostringstream buffer;

    append_timestamp_(buffer, e.time);
    buffer << " | ";
    append_level_(buffer, e.level);
    buffer << " | " << e.msg;
    append_locate_(buffer, e.file, e.line);

    append_stream_(buffer.str());
  }

  void basic_ostream_logger::append_stream_(const std::string &buffer)
  {
    stream_ << buffer << std::endl;
  }

  void basic_ostream_logger::append_timestamp_(std::ostream& stream, const std::chrono::high_resolution_clock::time_point &time)
  {
    std::chrono::high_resolution_clock::duration duration_from_epoch = time.time_since_epoch();
    std::time_t date = std::chrono::duration_cast<std::chrono::seconds>(duration_from_epoch).count();
    int          ms  = std::chrono::duration_cast<std::chrono::milliseconds>(duration_from_epoch).count() - date*1000;

    char tmp[20];
    std::strftime(tmp, 20, "%H:%M:%S", localtime (&date));

    stream << tmp << "." << std::setfill('0') << std::setw(3) << ms;
  }

  void basic_ostream_logger::append_level_(std::ostream &stream, log_level level)
  {
    stream << std::left << std::setw(9) << std::setfill(' ') << level;
  }

  void basic_ostream_logger::append_locate_(std::ostream &stream, const char *file, int line)
  {
    if(enable_locate_ && (file != nullptr) && line != -1)
    {
      stream << " (file:" << file << " line:" << line << ")";
    }
  }

  //
  // basic_fstream_logger
  //
  basic_fstream_logger::basic_fstream_logger(const std::string &filename, const std::string &filename_ext)
    : basic_ostream_logger(stream_)
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

  void basic_fstream_logger::set_max_line_by_file(int nb)
  {
    if(! stream_.is_open())
    {
      max_line_by_file_ = nb;
    }
  }

  void basic_fstream_logger::set_max_file_size(int size)
  {
    if(! stream_.is_open())
    {
      max_file_size_ = size;
    }
  }

  void basic_fstream_logger::append_stream_(const std::string &buffer)
  {
    int buffer_size = buffer.size();
    if(!stream_.is_open()
       || (max_line_by_file_ != -1 ? (nb_line_ + 1 >= max_line_by_file_) : false)
       || (max_file_size_ != -1 ? (size_file_ + buffer_size >= max_file_size_) : false))
    {
      create_new_file_();
    }

    basic_ostream_logger::append_stream_(buffer);

    // update counter
    if(max_line_by_file_ != -1 || max_file_size_ != -1)
    {
      ++nb_line_;
      size_file_ += buffer_size;
    }
  }

  void basic_fstream_logger::create_new_file_()
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
}
