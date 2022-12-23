#pragma once
#include <assert.h>
#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>

enum class Registers : int
{
  RAX = 0,
  RDI, 
  RSI, 
  RDX, 
  R8,  
  R9,  
  R10, 
  R11, 
  RSP, 
  RBX, 
  RBP, 
  R12, 
  R13, 
  R14, 
  R15, 
  RIP, 
  RegisterCount
};

extern "C"
{
  void coroutine_entry();
  void coroutine_switch(uint64_t *save, uint64_t *restore);
}

struct basic_context
{
  uint64_t *stack;                                          
  uint64_t stack_size;                                      
  uint64_t caller_registers[(int)Registers::RegisterCount]; 
  uint64_t callee_registers[(int)Registers::RegisterCount]; 
  bool finished;                                            
  bool ready;
  std::function<bool()> ready_func; 

  basic_context(uint64_t stack_size) 
      : finished(false), ready(true), stack_size(stack_size)
  {                                   
    stack = new uint64_t[stack_size]; 

    
    
    

    
    uint64_t rsp = (uint64_t)&stack[stack_size - 1];
    rsp = rsp - (rsp & 0xF); 
    
    void coroutine_main(struct basic_context * context);

    callee_registers[(int)Registers::RSP] = rsp; 
    
    callee_registers[(int)Registers::RIP] = (uint64_t)coroutine_entry; 
     

    
    callee_registers[(int)Registers::R12] = (uint64_t)coroutine_main; 
    
    callee_registers[(int)Registers::R13] = (uint64_t)this;
  } 

  ~basic_context() { delete[] stack; }

  virtual void run() = 0;
  virtual void resume() = 0;
};



void coroutine_main(struct basic_context *context) 
{
  context->run(); 
  context->finished = true;
  coroutine_switch(context->callee_registers, context->caller_registers); 

  
  assert(false);
}

extern __thread basic_context *g_current_context;


#define EXPAND_CALL_0(args)
#define EXPAND_CALL_1(args) (std::get<0>(args))
#define EXPAND_CALL_2(args) EXPAND_CALL_1(args), (std::get<1>(args))
#define EXPAND_CALL_3(args) EXPAND_CALL_2(args), (std::get<2>(args))
#define EXPAND_CALL_4(args) EXPAND_CALL_3(args), (std::get<3>(args))
#define EXPAND_CALL_5(args) EXPAND_CALL_4(args), (std::get<4>(args))
#define EXPAND_CALL_6(args) EXPAND_CALL_5(args), (std::get<5>(args))
#define EXPAND_CALL_7(args) EXPAND_CALL_6(args), (std::get<6>(args))

#define CALLER_IMPL(func, x, args)                                    \
  if constexpr (std::tuple_size_v<std::decay_t<decltype(args)>> == x) \
  func(EXPAND_CALL_##x(args))

#define CALL(func, args)      \
  CALLER_IMPL(func, 0, args); \
  CALLER_IMPL(func, 1, args); \
  CALLER_IMPL(func, 2, args); \
  CALLER_IMPL(func, 3, args); \
  CALLER_IMPL(func, 4, args); \
  CALLER_IMPL(func, 5, args); \
  CALLER_IMPL(func, 6, args); \
  CALLER_IMPL(func, 7, args);

template <typename F, typename... Args>
struct coroutine_context : public basic_context
{
  F f;                      
  std::tuple<Args...> args; 

  
  coroutine_context(F f, Args... args)
      : f(f), args(std::tuple<Args...>(args...)),
        basic_context(16 * 1024 / sizeof(uint64_t))
  {
    static_assert(sizeof...(args) <= 7);
  }

  
  coroutine_context(uint64_t stack_size, F f, Args... args)
      : f(f), args(std::tuple<Args...>(args...)),
        basic_context(stack_size * 1024 / sizeof(uint64_t))
  {
    static_assert(sizeof...(args) <= 7);
  }

 
  virtual void resume()
  {
    coroutine_switch(this->caller_registers, this->callee_registers);
    
  }

  virtual void run() { CALL(f, args); }
};