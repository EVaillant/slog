#ifndef SLOG_DETAIL_APPEND_WRITER_TO_LOGSTREAM_HPP
# define SLOG_DETAIL_APPEND_WRITER_TO_LOGSTREAM_HPP

# include <slog/detail/for_each_tag.hpp>
# include <slog/log_stream.hpp>
# include <slog/tag.hpp>

namespace slog
{
  namespace detail
  {
    struct fonctor_append_writer_to_logstream
    {
        template <class Tag> static void exec(LogStream &stream, Level level)
        {
          append_writter_(stream, level, Tag::writers);
        }

        static void default_value(LogStream &stream, Level level)
        {
          append_writter_(stream, level, get_default_writer());
        }

        static void append_writter_(LogStream &stream, Level level, const slog::level_writers & level_writers)
        {
          for(const auto & level_writer : level_writers)
          {
            if(level >= level_writer.first)
            {
              stream.append_writer(level_writer.second);
            }
          }
        }
    };

    template <class ... Tag> void append_writer_to_logstream(LogStream &stream, Level level)
    {
      detail::for_each_tag<detail::fonctor_append_writer_to_logstream, Tag...>::exec(stream, level);
    }
  }
}

#endif
