#ifndef SLOG_DETAIL_FOR_EACH_TAG_HPP
# define SLOG_DETAIL_FOR_EACH_TAG_HPP

# include <utility>
# include <tuple>

# include <slog/detail/expand_tag.hpp>

namespace slog
{
  namespace detail
  {
    template <class Tag> class for_each_tag
    {
      public:
        typedef typename expand_tag<Tag>::type expand_tag_type;
        template <std::size_t I> using         tag_type = typename std::tuple_element<I, expand_tag_type>::type;
        static const std::size_t               expand_tag_size = std::tuple_size<std::decay_t<expand_tag_type>>::value;

        template <class F, class ... Args> auto operator()(F && f, Args&& ... args)
        {
          return invoke_(std::forward<F>(f), std::make_index_sequence<expand_tag_size>{}, std::forward<Args>(args)...);
        }

      private:
        template <class F, class ... Args, std::size_t ... I> auto invoke_(F && f, std::index_sequence<I...>, Args&& ... args)
        {
          return f.template operator()<tag_type<I>...>(std::forward<Args>(args)...);
        }
    };
  }
}

#endif
