#pragma once
#include "context.h"
#include "coroutine_pool.h"
#include <cstdlib>

auto get_time() { return std::chrono::system_clock::now(); }

void yield() {
  if (!g_pool->is_parallel) {
    auto context = g_pool->coroutines[g_pool->context_id];  
    coroutine_switch(context->callee_registers, context->caller_registers); 
  }
}
void sleep(uint64_t ms) {
  if (g_pool->is_parallel) {
    auto cur = get_time();
    while (
        std::chrono::duration_cast<std::chrono::milliseconds>(get_time() - cur)
            .count() < ms)
      ;
  } else {
    auto context = g_pool->coroutines[g_pool->context_id]; 
    context->ready = false;
    auto cur = get_time();
    context->ready_func = [cur, ms]() {
      return std::chrono::duration_cast<std::chrono::milliseconds>(get_time() - cur).count() >= ms;
    };
    yield();
  }
}