#ifndef SLOG_TAG_HPP
# define SLOG_TAG_HPP

# include <slog/common.hpp>

namespace slog
{
  template <class T> struct tag
  {
    /**
     * @brief if false disable log
     */
    static bool enable;

    /**
     * @brief tag id
     */ 
    static const uint32_t id;
  };

  /**
   * @brief alias of tags
   *
   * exemple
   *
   *    struct my_tag1 : public tag<my_tag1> {};
   *    struct my_tag2 : public tag<my_tag2> {};
   *
   *    typedef my_super_tag tag_alias<my_tag1, my_tag2>;
   */
  template <class ... Tag> struct tag_alias
  {
  };

  /**
   * @brief is all tag is enable.
   */
  template <class Tag> bool is_enable();

  /**
   * @brief set enable status to all tag
   */
  template <class Tag> void set_enable(bool value);


  /**
   * @brief make bit mask from a tag
   */
  template <class Tag> mask_tag_type make_mask_tag(); 
}

# include <slog/tag.inl>

#endif
