#include <slog/logger_manager.hpp>
#include <cassert>

namespace slog
{
  logger_manager::logger_manager()
    : loggers_lock_(false)
    , sync_log_(true)
    , stop_async_thread_(false)
  {
    for(std::size_t i = 0 ; i < max_logger ; ++i)
    {
      loggers_[i] = nullptr;
    }
  }

  logger_manager::~logger_manager()
  {
    stop_async_logger();
    for(std::size_t i = 0 ; i < max_logger ; ++i)
    {
      if(loggers_[i] != nullptr)
      {
        if(auto_delete_[i])
        {
          delete loggers_[i];
          loggers_[i] = nullptr;
        }
        assert(loggers_[i] == nullptr);
      }
    }
  }

  logger_manager::self_type& logger_manager::instance()
  {
    static logger_manager instance_;
    return instance_;
  }

  logger_manager::logger_id_type logger_manager::append_logger(logger* l, bool auto_delete /*= false*/)
  {
    logger_id_type id = invalid_logger_id;
    guard_lock_logger guard(*this);
    for(std::size_t i = 0 ; i < max_logger ; ++i)
    {
      logger*& log = loggers_[i];    
      if(log == nullptr)
      {
        filter& f = filters_[i];
        bool& ad  = auto_delete_[i];
        f.activate = true;
        log = l;
        id  = i;
        ad  = auto_delete;
        break;
      }
    }   
    return id;
  }
  
  logger* logger_manager::remove_logger(logger_id_type id)
  {
    logger* ret = nullptr;
    guard_lock_logger guard(*this);

    logger*& log = loggers_[id];
    filter&    f = filters_[id];
    bool&     ad = auto_delete_[id];
    if(log != nullptr)
    {
      ret = log;
      if(ad)
      {
        delete ret;
      }
      log = nullptr;
      f.reset();
    }

    return ret;
  }

  void logger_manager::async_logger()
  {
    if(sync_log_ || stop_async_thread_)
    {
      sync_log_ = false;
      stop_async_thread_ = false;
      async_thread_id_   = std::thread(std::bind(&self_type::worker_, this)); 
    }
  }
  
  void logger_manager::stop_async_logger()
  {
    if(!sync_log_ && stop_async_thread_)
    {
      stop_async_thread_ = true;
      async_thread_id_.join(); 
    }
  }

  void logger_manager::notif_log(entry&& e)
  {
    if(sync_log_)
    {
      log_(e); 
    }
    else
    {
      while(!queue_.push(std::forward<entry>(e)))
      {
        little_sleep_();
      }
    }
  }
  
  void logger_manager::worker_()
  {
    while(!stop_async_thread_)
    {
      entry e;
      while(queue_.pop(e))
      {
        log_(e);
      }
      little_sleep_();
    }
  }
  
  void logger_manager::log_(const entry& e)
  {
    guard_lock_logger guard(*this);
    for(std::size_t i = 0 ; i < max_logger ; ++i)
    {
      filter& f = filters_[i];
      if(f.accept(e.level, *e.mask))
      {
        logger* log = loggers_[i];
        assert(log);
        log->write(e);
      }
    }
  }

  void logger_manager::lock_()
  {
    bool old;
    bool first = true;

    do 
    {
      old = false;
      if(!first)
      {
        little_sleep_();
      }
      else
      {
        first = false;
      }
    } while(!loggers_lock_.compare_exchange_weak(old, true));
  }
  
  void logger_manager::unlock_()
  {
    loggers_lock_ = false;
  }

  void logger_manager::little_sleep_()
  {
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
  }
}
