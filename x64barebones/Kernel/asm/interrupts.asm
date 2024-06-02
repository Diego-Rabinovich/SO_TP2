GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt
GLOBAL sysCallHandler
GLOBAL loadUserContext
GLOBAL dumpRegs
GLOBAL stored

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq06Handler

GLOBAL _exception00Handler
GLOBAL _exception06Handler

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN sysCallDispatcher
EXTERN infoRegs
EXTERN getStackBase
EXTERN Schedule



SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro pushStateNoRAX 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popStateNoRAX 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro



%macro saveRegs 0
    mov [reg_rax], rax
    mov rax, [rsp]
    mov [reg_rip], rax
    mov rax,[reg_rax]
    mov [reg_rdi], rdi
    mov [reg_rsi], rsi
    mov [reg_rbx], rbx
    mov [reg_r8], r8
    mov [reg_r9], r9
    mov [reg_r10], r10
    mov [reg_r11], r11
    mov [reg_r12], r12
    mov [reg_r12], r13
    mov [reg_r13], r14
    mov [reg_r15], r15
    mov [reg_rsp], rsp
    mov [reg_rbp], rbp
    mov DWORD [saved],1
%endmacro


%macro exceptionHandler 1
	pushState
	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher
	popState
    call loadUserContext
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp

    mov ax, di
    out	21h,al

    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
    saveRegs
    xor rax, rax
    in al, 0x60
    cmp rax,0x29 ;scan code de `
    jne .continue
    mov al, 20h
    out 20h, al
    call infoRegs
    .continue:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;SysCalls
_irq06Handler:

    pushStateNoRAX

    push rax
    call _sti
    call sysCallDispatcher
    add rsp, 8

    popStateNoRAX
	iretq


;Zero Division Exception
_exception00Handler:
    saveRegs
	exceptionHandler 0

_exception06Handler:
    saveRegs
    exceptionHandler 6

loadUserContext:
    ;ret contextToLoad
    mov rax, rdi
    mov [rsp], rax
    ;Code Segment
    mov rax, 0x8
    mov [rsp + 8], rax
    ;RFLAGS
    mov rax, 0x202
    mov [rsp + 8*2], rax
    ;StackBase
    call getStackBase
    mov [rsp + 8*3], rax
    ;Stack Segment
    mov rax, 0x0
    mov [rsp + 8*4], rax
    iretq

haltcpu:
	cli
	hlt
	ret


dumpRegs:
    mov DWORD [saved],0
    mov rax,regStruct
    ret

stored:
    mov rax,[saved]
    ret

scheduler_handler:
    pushState
    mov rdi, rsp
    call Schedule
    mov rsp, rax
    mov al, 20h
    out 20h, al
    popState
    iretq

section .data
saved dd 0
regStruct:
    reg_rdi dq 0
    reg_rsi dq 0
    reg_rax dq 0
    reg_rbx dq 0
    reg_r8 dq 0
    reg_r9 dq 0
    reg_r10 dq 0
    reg_r11 dq 0
    reg_r12 dq 0
    reg_r13 dq 0
    reg_r14 dq 0
    reg_r15 dq 0
    reg_rip dq 0
    reg_rsp dq 0
    reg_rbp dq 0

SECTION .rodata
    userLand equ 0x400000

SECTION .bss
	aux resq 1
