#ifndef SLOG_DETAIL_FOR_EACH_TAG_HPP
# define SLOG_DETAIL_FOR_EACH_TAG_HPP

# include <utility>
# include <type_traits>

# include <slog/tag.hpp>

namespace slog
{
  namespace detail
  {
    template <class F, class ... Tag> struct for_each_tag
    {
    };

    template <class F, class Tag> struct for_each_tag<F, Tag>
    {
        template <class ... Args> static auto exec(Args&& ... args) -> decltype(F::template exec<Tag>(std::forward<Args>(args)...))
        {
          return F::template exec<Tag>(std::forward<Args>(args)...);
        }
    };

    template <class F> struct for_each_tag<F>
    {
        template <class ... Args> static auto exec(Args&& ... args) -> decltype(F::default_value(std::forward<Args>(args)...))
        {
          return F::default_value(std::forward<Args>(args)...);
        }
    };

    template <class F, class ... Tag> struct for_each_tag<F, slog::tag_alias<Tag...> >
    {
        template <class ... Args> static auto exec(Args && ... args) -> decltype(for_each_tag<F, Tag...>::exec(std::forward<Args>(args)...))
        {
          return for_each_tag<F, Tag...>::exec(std::forward<Args>(args)...);
        }
    };

    template <class F, class T0, class ... Tag> struct for_each_tag<F, T0, Tag...>
    {
        template <class ... Args> static auto exec(Args && ... args) ->
          typename std::enable_if<
            !std::is_same<
              decltype(for_each_tag<F, T0>::exec(std::forward<Args>(args)...)),
              void
            >::value,
            decltype(for_each_tag<F, T0>::exec(std::forward<Args>(args)...))
          >::type
        {
          return F::merge(for_each_tag<F, T0>::exec(std::forward<Args>(args)...), for_each_tag<F, Tag...>::exec(std::forward<Args>(args)...));
        }

        template <class ... Args> static auto exec(Args && ... args) ->
          typename std::enable_if<
            std::is_same<
              decltype(for_each_tag<F, T0>::exec(std::forward<Args>(args)...)),
              void
            >::value
          >::type
        {
          for_each_tag<F, T0>::exec(std::forward<Args>(args)...);
          for_each_tag<F, Tag...>::exec(std::forward<Args>(args)...);
        }
    };
  }
}

#endif
