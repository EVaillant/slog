#ifndef SLOG_TAG_HPP
# error "must not be included directly"
#endif

# include <slog/detail/for_each_tag.hpp>
# include <functional>

namespace slog
{
  namespace detail
  {
    inline uint32_t& last_tag_id()
    {
      static uint32_t id = 0;
      return id;
    }

    inline uint32_t make_tag_id()
    {
      static uint32_t &id = last_tag_id();
      assert(id < nb_max_tag);
      return id++;
    }

    template <class T> uint32_t get_tag_id()
    {
      static uint32_t id = make_tag_id();
      return id;
    }

    struct is_enable_impl
    {
      template <class ... Tag> bool operator()()
      {
        bool ret = true;
        static std::reference_wrapper<bool> values[] = {std::ref(Tag::enable)...};
        for(bool& value : values)
        {
          ret = ret && value;
        }      
        return ret;
      }
    };

    struct set_enable_impl
    {
      template <class ... Tag> void operator()(bool v)
      {
        static std::reference_wrapper<bool> values[] = {std::ref(Tag::enable)...};
        for(bool& value : values)
        {
          value = v;
        }
      }
    };

    struct make_mask_impl
    {
      template <class ... Tag> mask_tag_type operator()()
      {
        static std::reference_wrapper<const uint32_t> ids[] = {std::cref(Tag::id)...};
        mask_tag_type mask;
        for(const uint32_t& id : ids)
        {
          mask.set(id);
        }
        return mask;
      }
    };
  }

  template <class T>           bool tag<T>::enable = true;
  template <class T> const uint32_t tag<T>::id     = detail::get_tag_id<T>();

  template <class Tag> bool is_enable()
  {
    detail::is_enable_impl f;
    detail::for_each_tag<Tag> for_each;
    return for_each(f);
  }

  template <class Tag> void set_enable(bool value)
  {
    detail::set_enable_impl f;
    detail::for_each_tag<Tag> for_each;
    return for_each(f, value);
  }

  template <class Tag> mask_tag_type make_mask_tag()
  {
    detail::make_mask_impl f;
    detail::for_each_tag<Tag> for_each;
    return for_each(f);
  }
}
