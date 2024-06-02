GLOBAL cpuVendor
GLOBAL cpuGetTime
GLOBAL getKeyPress
GLOBAL inb
GLOBAL outb
GLOBAL callTimerTick

section .text

cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

cpuGetTime:
	push rbp
	mov rbp, rsp
	mov rax, 0
   	mov al, 4     ; hours.
	out 70h, al       ; 70h entrada para la informacion que quiero en 71h.
   	in al, 71h           ; en ax se deposita lo pedido por 70h, presente en 71h.
	mov [hour], rax

	mov rax, 0
   	mov al, 2     ; minutes.
	out 70h, al       ; 70h entrada para la informacion que quiero en 71h.
   	in al, 71h           ; en ax se deposita lo pedido por 70h, presente en 71h.
	mov [min], rax

	mov rax, 0
	out 70h, al       ; 70h entrada para la informacion que quiero en 71h.
   	in al, 71h           ; en ax se deposita lo pedido por 70h, presente en 71h.
	mov [sec], rax
	mov rax, timeStruct

	mov rsp, rbp
	pop rbp
	ret

getKeyPress:
	enter 0,0

	xor rax, rax
	in al, 0x60

	leave
	ret
        reg_rip dq 0
        reg_rsp dq 0
        reg_rbp dq 0


outb:
	mov eax, esi
	mov edx, edi
	out dx, al
	ret

inb:
	mov edx, edi
	in	al, dx
	ret


section .bss

timeStruct:
    hour resb 4
    min resb 4
    sec resb 4

callTimerTick:
    int 20h
