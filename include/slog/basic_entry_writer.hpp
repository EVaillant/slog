#ifndef SLOG_BASIC_ENTRY_WRITER_OSTREAM_HPP
# define SLOG_BASIC_ENTRY_WRITER_OSTREAM_HPP

# include <slog/ientry_writer.hpp>
# include <slog/level.hpp>

# include <fstream>
# include <thread>
# include <chrono>
# include <list>
# include <mutex>
# include <condition_variable>

namespace slog
{
  /**
   * @brief The BasicEntryWriterOStream class is genreric stream adapter on IEntryWriter
   *        Log format is :
   *          %HH:%MM:%SS.MMMM | Level | log msg
   *        or is locate is enable :
   *          %HH:%MM:%SS.MMMM | Level | log msg (file:, line:)
   */
  class BasicEntryWriterOStream : public IEntryWriter
  {
    public:
      /**
       * @brief Ctor BasicEntryWriterOStream
       * @param stream stream used to log
       */
      BasicEntryWriterOStream(std::ostream &stream);

      /**
       * @brief set_locate to enable/disable locate in log msg
       * @param value
       */
      void set_locate(bool value);

      /**
       * @see IEntryWriter::write
       */
      virtual void write(const std::shared_ptr<Entry>& entry) override;

    protected:
      /**
       * @brief append_stream_ append buffer to internal stream
       * @param buffer buffer to add
       */
      virtual void append_stream_(const std::string &buffer);

      void append_timestamp_(std::ostream &stream, const std::chrono::high_resolution_clock::time_point &time);
      void append_level_(std::ostream& stream,  Level level);
      void append_locate_(std::ostream& stream, const std::string &file, int line);

    private:
      std::ostream &stream_;
      bool enable_locate_;
  };

  /**
   * @brief The BasicEntryWriterFStream class is file streamer adapter on IEntryWriter
   *
   * file format :
   *    filename_<hostname>_<pid>_<date>.filename_ext
   * or
   *    filename_<hostname>_<pid>_<date>_<counter>.filename_ext
   *
   * to have counter set_max_line_by_file or set_max_file_size must be call with value diferrent -1.
   */
  class BasicEntryWriterFStream : public BasicEntryWriterOStream
  {
    public:
      /**
       * @brief Ctor BasicEntryWriterFStream
       * @param filename  filename
       * @param filename_ext filename extention
       */
      BasicEntryWriterFStream(const std::string& filename, const std::string &filename_ext);

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
       * @see BasicEntryWriterOStream::append_stream_
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

  /**
   * @brief The AsyncEntryWriter class is adapter override IEntryWriter to process asynchrone write
   */
  class AsyncEntryWriter : public IEntryWriter
  {
    public:
      /**
       * @brief Ctor AsyncEntryWriter
       * @param writer
       *
       * start thread
       */
      AsyncEntryWriter(std::unique_ptr<IEntryWriter> && writer);

      /**
       * @brief Dtor AsyncEntryWriter
       *
       * stop thread
       */
      ~AsyncEntryWriter();

      /**
       * @see IEntryWriter::write
       */
      virtual void write(const std::shared_ptr<Entry>& entry);

    private:
      void async_write_();
      std::shared_ptr<Entry> get_first_entry_();

      std::list<std::shared_ptr<Entry>> entries_;
      std::condition_variable entries_cond_;
      std::mutex entries_mutex_;

      std::unique_ptr<IEntryWriter> writer_;
      std::thread thread_;
      volatile bool quit_;
  };
}

#endif
