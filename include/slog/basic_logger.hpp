#ifndef SLOG_BASIC_LOGGER_HPP
# define SLOG_BASIC_LOGGER_HPP

# include <slog/common.hpp>

# include <fstream>

namespace slog
{
  /**
   * @brief The basic_ostream_logger class is genreric stream adapter on logger
   *        Log format is :
   *          %HH:%MM:%SS.MMMM | Level | log msg
   *        or is locate is enable :
   *          %HH:%MM:%SS.MMMM | Level | log msg (file:, line:)
   */
  class basic_ostream_logger : public logger
  {
    public:
      /**
       * @brief Ctor basic_ostream_logger
       * @param stream stream used to log
       */
      basic_ostream_logger(std::ostream &stream);

      /**
       * @brief set_locate to enable/disable locate in log msg
       * @param value
       */
      void set_locate(bool value);

      /**
       * @see logger::write
       */
      virtual void write(const entry& entry) override;

    protected:
      /**
       * @brief append_stream_ append buffer to internal stream
       * @param buffer buffer to add
       */
      virtual void append_stream_(const std::string &buffer);

      void append_timestamp_(std::ostream &stream, const std::chrono::high_resolution_clock::time_point &time);
      void append_level_(std::ostream& stream, log_level level);
      void append_locate_(std::ostream& stream, const char* file, int line);

    private:
      std::ostream &stream_;
      bool enable_locate_;
  };

  /**
   * @brief The basic_fstream_logger class is file streamer adapter on logger
   *
   * file format :
   *    filename_<hostname>_<pid>_<date>.filename_ext
   * or
   *    filename_<hostname>_<pid>_<date>_<counter>.filename_ext
   *
   * to have counter set_max_line_by_file or set_max_file_size must be call with value diferrent -1.
   */
  class basic_fstream_logger : public basic_ostream_logger 
  {
    public:
      /**
       * @brief Ctor basic_fstream_logger
       * @param filename  filename
       * @param filename_ext filename extention
       */
      basic_fstream_logger(const std::string& filename, const std::string &filename_ext);

      /**
       * @brief set_max_line_by_file set nb max line by file
       * @param nb nb max de line
       *
       * @warning can only by set before to start to used it
       */
      void set_max_line_by_file(int nb);

      /**
       * @brief set_max_file_size set max size by file
       * @param size size max
       *
       * @warning can only by set before to start to used it
       */
      void set_max_file_size(int size);

    protected:
      /**
       * @see basic_ostream_logger::append_stream_
       */
      virtual void append_stream_(const std::string &buffer) override;

      void create_new_file_();

    private:
      std::string filename_;
      std::string filename_ext_;

      int file_counter_;
      int nb_line_;
      int size_file_;

      int max_line_by_file_;
      int max_file_size_;

      std::fstream stream_;
  };
}

#endif
