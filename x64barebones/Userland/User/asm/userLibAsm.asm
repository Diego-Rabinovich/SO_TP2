GLOBAL sys_read ;bloqueante
GLOBAL sys_write ;bloqueante
GLOBAL sys_paintPixel
GLOBAL sys_getScreenDimensions
GLOBAL sys_getCpuTime
GLOBAL sys_resetCursor
GLOBAL sys_blankArea
GLOBAL sys_waitNMillis ;bloqueante
GLOBAL sys_hardRead ;AFUERAAAAAAAAAA
GLOBAL sys_beep
GLOBAL sys_malloc
GLOBAL sys_free
GLOBAL sys_createProcess
GLOBAL sys_memset
GLOBAL sys_kill
GLOBAL sys_block
GLOBAL sys_unblock
GLOBAL sys_nice
GLOBAL sys_pid
GLOBAL sys_yield
GLOBAL sys_wait_pid
GLOBAL sys_ps
GLOBAL sys_get_FDs
GLOBAL sys_sem_new
GLOBAL sys_sem_destroy
GLOBAL sys_sem_post
GLOBAL sys_sem_wait
GLOBAL sys_openFd
GLOBAL sys_closeFd
GLOBAL sys_createFd
GLOBAL sys_memInfo
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

sys_createProcess:
    mov rax, 13
    int 80h
    ret

sys_kill:
    mov rax, 14
    int 80h
    ret

sys_block:
    mov rax, 15
    int 80h
    ret

sys_unblock:
    mov rax, 16
    int 80h
    ret

sys_nice:
    mov rax, 17
    int 80h
    ret

sys_pid:
    mov rax, 18
    int 80h
    ret

sys_yield:
     mov rax, 19
     int 80h
     ret

sys_wait_pid:
    mov rax, 20
    int 80h
    ret

sys_ps:
    mov rax, 21
    int 80h
    ret

sys_get_FDs:
    mov rax, 22
    int 80h
    ret

sys_sem_new:
    mov rax, 23
    int 80h
    ret

sys_sem_destroy:
    mov rax, 24
    int 80h
    ret

sys_sem_post:
    mov rax, 25
    int 80h
    ret

sys_sem_wait:
    mov rax, 26
    int 80h
    ret

sys_openFd:
    mov rax, 27
    int 80h
    ret

sys_closeFd:
    mov rax, 28
    int 80h
    ret

sys_createFd:
    mov rax, 29
    int 80h
    ret

sys_memInfo:
    mov rax, 30
    int 80h
    ret

make_invalid_opCode:
    ud2
    ret
