#ifndef SLOG_DETAIL_EXPAND_TAG_HPP
# define SLOG_DETAIL_EXPAND_TAG_HPP

# include <tuple>

namespace slog
{
  namespace detail
  {
    template <class Tag> struct expand_tag
    {
      typedef std::tuple<Tag> type;
    };

    template <class ... Tags> struct expand_tag<tag_alias<Tags...>>
    {
      typedef decltype(std::tuple_cat(typename expand_tag<Tags>::type()...)) type;
    };
  }
}

#endif
