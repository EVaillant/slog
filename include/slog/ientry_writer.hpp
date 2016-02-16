#ifndef SLOG_IENTRY_WRITER_HPP
# define SLOG_IENTRY_WRITER_HPP

# include <memory>

namespace slog
{
  struct Entry;

  /**
   * @brief The IEntryWriter class is interface to log writer
   */
  class IEntryWriter
  {
    public:
      /**
       * @brief Dtor IEntryWriter
       */
      virtual ~IEntryWriter(){}

      /**
       * @brief write on entry
       * @param entry to write
       */
      virtual void write(const std::shared_ptr<Entry>& entry) = 0;
  };
}

#endif
