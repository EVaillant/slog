#ifndef SLOG_TAG_HPP
# define SLOG_TAG_HPP

# include <string>
# include <list>

# include <slog/level.hpp>
# include <slog/ientry_writer.hpp>

namespace slog
{
  typedef std::list<std::pair<Level, std::shared_ptr<IEntryWriter>>> level_writers;

  /**
   * @brief tag
   */
  template <class T> struct tag
  {
      /**
       * @brief if false disable log
       */
      static bool enable;

      /**
       * @brief prefix to add to log
       */
      static std::string prefix;

      /**
       * @brief list of level + writer
       */
      static level_writers writers;
  };
  template <class T> bool        tag<T>::enable = true;
  template <class T> std::string tag<T>::prefix = "";
  template <class T> level_writers tag<T>::writers;

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
  template <class ... Tag> bool is_enable();

  /**
   * @brief set enable status to all tag
   */
  template <class ... Tag> void set_enable(bool value);

  /**
   *
   */
  template <class ... Tag> std::string get_prefix();
  template <class ... Tag> void set_prefix(const std::string &value);

  /**
   *
   */
  template <class ... Tag> void append_writter(Level level, const std::shared_ptr<IEntryWriter>& writer);
  template <class ... Tag> void clear_writter();

  /**
   *
   */
  void append_default_writer(Level level, const std::shared_ptr<IEntryWriter>& writer);
  void clear_default_writer();
  const level_writers& get_default_writer(void);
}

# include <slog/tag.inl>

#endif
