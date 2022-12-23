#pragma once
#include "context.h"
#include <memory>
#include <thread>
#include <vector>

struct coroutine_pool;
extern coroutine_pool *g_pool;
struct coroutine_pool
{
  std::vector<basic_context *> coroutines;
  int context_id; 

  
  bool is_parallel;

  ~coroutine_pool()
  {
    for (auto context : coroutines)
    {
      delete context;
    }
  }

  
  template <typename F, typename... Args>
  void new_coroutine(F f, Args... args)
  {
    coroutines.push_back(new coroutine_context(f, args...));
  }

  void parallel_execute_all()
  {
    g_pool = this;
    is_parallel = true;
    std::vector<std::thread> threads;
    for (auto p : coroutines)
    {
      threads.emplace_back([p]()
                           { p->run(); });
    }

    for (auto &thread : threads)
    {
      thread.join();
    }
  }

void serial_execute_all()
  {
    is_parallel = false;
    g_pool = this;
    while(true) 
    {
      bool all_finished = true;
      for(int i = 0 ; i < coroutines.size() ; i++)
      {
        if(coroutines[i]->ready && !coroutines[i]->finished){
          all_finished = false;
          context_id = i;
          coroutines[i]->resume();
        } else if(!coroutines[i]->ready){
                coroutines[i]->ready = coroutines[i]->ready_func();
                };

          }
          if(all_finished) break;
        }
    

    for (auto context : coroutines)
    {
      delete context; 
    }
    coroutines.clear();
};
};
