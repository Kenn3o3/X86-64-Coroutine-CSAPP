.global coroutine_entry
coroutine_entry:
    movq %r13, %rdi
    callq *%r12

.global coroutine_switch
coroutine_switch:
    leaq .coroutine_ret(%rip), %rax
    movq %rax, 120(%rdi)
    movq %rsp, 64(%rdi)
    movq %rbx, 72(%rdi)
    movq %rbp, 80(%rdi)
    movq %r12, 88(%rdi)
    movq %r13, 96(%rdi)
    movq %r14, 104(%rdi)
    movq %r15, 112(%rdi)

    movq 64(%rsi), %rsp
    movq 72(%rsi), %rbx
    movq 80(%rsi), %rbp
    movq 88(%rsi), %r12
    movq 96(%rsi), %r13
    movq 104(%rsi), %r14
    movq 112(%rsi), %r15
    jmpq *120(%rsi) 

.coroutine_ret: 
    ret
