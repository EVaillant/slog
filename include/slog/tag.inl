#ifndef SLOG_TAG_HPP
# error "must not be included directly"
#endif

#include <slog/detail/for_each_tag.hpp>
#include <slog/log_stream.hpp>

namespace slog
{
  namespace detail
  {
    struct fonctor_enable_impl
    {
        template <class Tag> static bool exec()
        {
          return Tag::enable;
        }

        static bool default_value()
        {
          return true;
        }

        template <class Tag> static void exec(bool val)
        {
          Tag::enable = val;
        }

        static bool merge(bool l, bool r)
        {
          return l && r;
        }
    };

    struct fonctor_prefix_impl
    {
        template <class Tag> static const std::string& exec()
        {
          return Tag::prefix;
        }

        static const std::string& default_value()
        {
          static std::string value = "";
          return value;
        }

        template <class Tag> static void exec(const std::string & val)
        {
          Tag::prefix = val;
        }

        static const std::string& merge(const std::string &l, const std::string& r)
        {
          return (l.empty() ? r : l);
        }
    };

    struct functor_append_writer
    {
        template <class Tag> static void exec(Level level, const std::shared_ptr<IEntryWriter>& writer)
        {
          return Tag::writter.append( std::make_pair(level, writer));
        }

        static void default_value(Level, const std::shared_ptr<IEntryWriter>&)
        {
        }
    };

    struct functor_clear_writer
    {
        template <class Tag> static void exec()
        {
          return Tag::writter.clear();
        }

        static void default_value()
        {
        }
    };
  }

  template <class ... Tag> bool is_enable()
  {
    return detail::for_each_tag<detail::fonctor_enable_impl, Tag...>::exec();
  }

  template <class ... Tag> void set_enable(bool value)
  {
    detail::for_each_tag<detail::fonctor_enable_impl, Tag...>::exec(value);
  }

  template <class ... Tag> std::string get_prefix()
  {
    return detail::for_each_tag<detail::fonctor_prefix_impl, Tag...>::exec();
  }

  template <class ... Tag> void set_prefix(const std::string &value)
  {
    detail::for_each_tag<detail::fonctor_prefix_impl, Tag...>::exec(value);
  }

  template <class ... Tag> void append_writter(Level level, const std::shared_ptr<IEntryWriter>& writer)
  {
    detail::for_each_tag<detail::functor_append_writer, Tag...>::exec(level, writer);
  }

  template <class ... Tag> void clear_writter()
  {
    detail::for_each_tag<detail::functor_clear_writer, Tag...>::exec();
  }
}
