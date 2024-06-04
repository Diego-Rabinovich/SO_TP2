GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_paintPixel
GLOBAL sys_getScreenDimensions
GLOBAL sys_getCpuTime
GLOBAL sys_resetCursor
GLOBAL sys_blankArea
GLOBAL sys_waitNMillis
GLOBAL sys_hardRead
GLOBAL sys_beep
GLOBAL sys_malloc
GLOBAL sys_free
GLOBAL sys_memset
GLOBAL make_invalid_opCode

section .text

sys_read:
    mov rax, 0
    int 80h
    ret

sys_write:
    mov rax, 1
    int 80h
    ret

sys_paintPixel:
    mov rax, 2
    int 80h
    ret

sys_getScreenDimensions:
    mov rax, 3
    int 80h
    ret

sys_getCpuTime:
    mov rax, 4
    int 80h
    ret

sys_resetCursor:
    mov rax, 5
    int 80h
    ret

sys_blankArea:
    mov rax, 6
    int 80h
    ret

sys_waitNMillis:
    mov rax, 7
    int 80h
    ret


sys_hardRead:
    mov rax, 8
    int 80h
    ret

sys_beep:
    mov rax, 9
    int 80h
    ret

sys_malloc:
    mov rax, 10
    int 80h
    ret
sys_free:
    mov rax, 11
    int 80h
    ret
sys_memset:
    mov rax, 12
    int 80h
    ret

make_invalid_opCode:
    ud2
    ret
