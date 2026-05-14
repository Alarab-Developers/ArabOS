bits 64

global isr_timer
global isr_keyboard
global isr_mouse
global isr_page_fault

extern irq_handler
extern current_process
extern scheduler_next
extern page_fault_handler_c
extern idle_p          ;  نحتاجه لتجنب حفظ rsp للـ idle

%macro SAVE_REGS 0
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
%endmacro

%macro RESTORE_REGS 0
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
%endmacro

; ================================================================
; isr_timer — IRQ0 → vector 32
;
;  الإصلاح:
;   - لا نحفظ rsp للعملية idle (تتبديل دائماً)
;   - نتحقق من scheduler_next قبل التبديل
;   - التبديل يحدث هنا فقط (لا في C بعد STI)
; ================================================================
isr_timer:
    SAVE_REGS

    ; احفظ RSP في current_process->rsp (إن وُجد وليس idle)
    mov r15, [rel current_process]
    test r15, r15
    jz .no_save

    ;  لا تحفظ rsp للـ idle — هي لا تحتاج استعادة
    mov r14, [rel idle_p]
    cmp r15, r14
    je .no_save

    mov [r15 + 16], rsp   ; p->rsp = rsp الحالي

.no_save:
    ; استدعِ irq_handler (يُرسل EOI ويستدعي scheduler_tick)
    mov rdi, 0
    call irq_handler

    ; احصل على العملية التالية
    call scheduler_next       ; rax = العملية التالية

    test rax, rax
    jz .done

    cmp rax, r15
    je .done                  ; نفس العملية → لا تبديل

    ; ── تبديل فعلي ──
    mov [rel current_process], rax
    mov rsp, [rax + 16]       ; حمّل RSP العملية الجديدة

.done:
    RESTORE_REGS
    iretq

; ================================================================
; isr_keyboard — IRQ1 → vector 33
; ================================================================
isr_keyboard:
    SAVE_REGS
    mov rdi, 1
    call irq_handler
    RESTORE_REGS
    iretq

; ================================================================
; isr_mouse — IRQ12 → vector 44
; ================================================================
isr_mouse:
    SAVE_REGS
    mov rdi, 12
    call irq_handler
    RESTORE_REGS
    iretq

; ================================================================
; isr_page_fault — interrupt 14
; ================================================================
isr_page_fault:
    SAVE_REGS
    mov rdi, rsp
    call page_fault_handler_c
    RESTORE_REGS
    add rsp, 8
    iretq
