#ifndef SLOG_LOGGER_MANAGER_HPP
# define SLOG_LOGGER_MANAGER_HPP

# include <toolsbox/lockfree/fix_size_queue.hpp>
# include <slog/common.hpp>
# include <array>
# include <atomic>
# include <thread>
# include <bitset>

namespace slog
{
  class logger_manager
  {
    public:
      typedef logger_manager self_type;
      typedef std::size_t    logger_id_type;

      static const std::size_t    max_logger = 32;
      static const std::size_t    async_queue_size = 1024;
      static const logger_id_type invalid_logger_id = -1;

      static self_type& instance();

      inline filter& get_filter(logger_id_type id)
      {
        return filters_[id];
      }

      logger_id_type append_logger(logger* l, bool auto_delete = false);
      logger* remove_logger(logger_id_type id);

      void async_logger();
      void stop_async_logger();

      void notif_log(entry&& e);

    protected:
      void worker_();
      void log_(const entry& e);

      void lock_();
      void unlock_();

      class guard_lock_logger
      {
        public:
          guard_lock_logger(self_type& owner)
            : owner_(owner)
          {
            owner_.lock_();
          }

          ~guard_lock_logger()
          {
            owner_.unlock_();
          }

        private:
          self_type& owner_;
      };

      void little_sleep_();

    private:
      typedef std::bitset<max_logger>         logger_mask_type;
      typedef std::array<filter,  max_logger> filters_type;
      typedef std::array<logger*, max_logger> loggers_type;
      typedef std::array<bool,    max_logger> auto_delete_type;
      typedef toolsbox::lockfree::fix_size_queue<entry, async_queue_size> queue_type;

      logger_manager();
      ~logger_manager();

      filters_type     filters_;
      loggers_type     loggers_;
      auto_delete_type auto_delete_;
      std::atomic_bool loggers_lock_;
      bool             sync_log_;

      queue_type       queue_;
      std::thread      async_thread_id_;
      std::atomic_bool stop_async_thread_;
  };
}

#endif
