; ============================================================
;  multiboot.asm — Higher Half Kernel (0xFFFF800000000000)
; ============================================================

section .multiboot2
align 8
header_start:
    dd 0xE85250D6
    dd 0
    dd header_end - header_start
    dd -(0xE85250D6 + 0 + (header_end - header_start))
    align 8
    dw 5
    dw 0
    dd 20
    dd 0
    dd 0
    dd 32
    align 8
    dw 0
    dw 0
    dd 8
header_end:

; ========== Bootstrap Data ==========
section .boot32data
align 4096
pml4_table: times 512 dq 0
pdpt_low:   times 512 dq 0
pdpt_high:  times 512 dq 0
pd_table_0: times 512 dq 0

align 8
gdt64:
    dq 0
.code_seg: equ $ - gdt64
    dq (1<<43)|(1<<44)|(1<<47)|(1<<53)
.data_seg: equ $ - gdt64
    dq (1<<41)|(1<<44)|(1<<47)
gdt64_ptr:
    dw $ - gdt64 - 1
    dd gdt64

; ========== Bootstrap Stack ==========
section .boot32bss nobits
align 16
stack_bottom: resb 16384
stack_top:

; ========== 32-bit Code ==========
section .boot32
bits 32
global _start
extern kernel_main

_start:
    cli
    mov edi, eax
    mov esi, ebx
    mov esp, stack_top
    call check_cpuid
    call check_long_mode
    call setup_page_tables

    mov eax, cr4
    or  eax, 1 << 5
    mov cr4, eax

    mov eax, pml4_table
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or  eax, 1 << 8
    wrmsr

    mov eax, cr0
    or  eax, (1<<31)|(1<<0)
    mov cr0, eax

    lgdt [gdt64_ptr]
    jmp gdt64.code_seg:long_mode_phys

check_cpuid:
    pushfd
    pop  eax
    mov  ecx, eax
    xor  eax, 1 << 21
    push eax
    popfd
    pushfd
    pop  eax
    push ecx
    popfd
    cmp  eax, ecx
    je   .no
    ret
.no: hlt

check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb  .no
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz  .no
    ret
.no: hlt

setup_page_tables:
    mov eax, pdpt_low
    or  eax, 0x03
    mov [pml4_table + 0*8], eax

    mov eax, pdpt_high
    or  eax, 0x03
    mov [pml4_table + 256*8], eax

    mov eax, pd_table_0
    or  eax, 0x03
    mov [pdpt_low + 0*8], eax

    mov eax, pd_table_0
    or  eax, 0x03
    mov [pdpt_high + 0*8], eax

    mov ecx, 0
.map_loop:
    mov eax, ecx
    shl eax, 21
    or  eax, 0x83
    mov [pd_table_0 + ecx*8], eax
    inc ecx
    cmp ecx, 512
    jne .map_loop
    ret

; ========== 64-bit Physical Entry ==========
bits 64
long_mode_phys:
    mov rax, 0xFFFF800000000000
    add rsp, rax
    mov rax, long_mode_virt
    jmp rax

; ========== 64-bit Virtual Entry ==========
section .text
bits 64
long_mode_virt:
    mov ax, gdt64.data_seg
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; ✅ محاذاة RSP لـ 16 bytes — مطلوب بـ x86-64 ABI
    and rsp, -16

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
