bits 64

global context_switch
global jump_to_first_task

; ================================================================
; context_switch
; void context_switch(uint64_t* old_rsp, uint64_t new_rsp)
;   rdi = &old_rsp
;   rsi = new_rsp
;
; تُستخدم للتبديل الأول من schedule() قبل STI.
; بعد STI كل التبديل يحدث عبر isr_timer.
;
; هيكل مكدس العملية الجديدة (مُعدّ في process_create):
;   [rsp+  0]  r15   ← أول pop
;   [rsp+  8]  r14
;   [rsp+ 16]  r13
;   [rsp+ 24]  r12
;   [rsp+ 32]  r11
;   [rsp+ 40]  r10
;   [rsp+ 48]  r9
;   [rsp+ 56]  r8
;   [rsp+ 64]  rbp
;   [rsp+ 72]  rdi
;   [rsp+ 80]  rsi
;   [rsp+ 88]  rdx
;   [rsp+ 96]  rcx
;   [rsp+104]  rbx
;   [rsp+112]  rax   ← آخر pop
;   ── iretq frame ──
;   [rsp+120]  RIP
;   [rsp+128]  CS
;   [rsp+136]  RFLAGS
;   [rsp+144]  RSP
;   [rsp+152]  SS
; ================================================================
context_switch:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov [rdi], rsp      ; احفظ RSP في *old_rsp
    mov rsp, rsi        ; حمّل RSP العملية الجديدة

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    iretq

; ================================================================
; jump_to_first_task
; void jump_to_first_task(uint64_t rsp)
;   rdi = RSP العملية الأولى
; ================================================================
jump_to_first_task:
    mov rsp, rdi

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    iretq
