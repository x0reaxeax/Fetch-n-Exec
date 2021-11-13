[BITS 64]

; https://github.com/x0reaxeax/x0l1b/blob/main/sys/syscalls.asm

global x_sys_write      ; sys_write(int fd, const char *buf, size_t count)
global x_sys_read       ; sys_read(int fd, void *outbuf, size_t count)
global x_sys_close      ; sys_close(int fd)
global x_sys_open       ; sys_open(const char *binpath, int flags)
global x_sys_mmap       ; sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
global x_sys_munmap     ; sys_munmap(void *addr, size_t length)
global x_sys_mprotect   ; sys_mprotect(unsigned long start, size_t len, unsigned long prot)
global x_sys_socket     ; sys_socket(int domain, int type, int protocol)
global x_sys_connect    ; sys_connect(int fd, const struct sockaddr *uservaddr, unsigned int addrlen)
global x_sys_sendto     ; sys_sendto(int fd, void *buff, size_t len, unsigned int flags, struct sockaddr *addr, int addr_len)
global x_sys_recvfrom   ; sys_recvfrom(int fd, void *ubuf, size_t size, uint flags, struct sockaddr *addr, int *addr_len)
global x_sys_shutdown   ; sys_shutdown(int fd, int how)
global x_sys_exit       ; sys_exit(int exit_code)

x_sys_write:
    mov     rax,    1   ; sys_write
    syscall
    ret

x_sys_read:
    xor     eax,    eax ; sys_read (0x0)
    syscall
    ret

x_sys_close:
    mov     eax,    0x3 ; sys_close
    syscall
    ret

x_sys_open:
    mov     eax,    0x2 ; sys_open
    syscall
    ret

x_sys_munmap:
    mov     eax,    0xb ; sys_munmap
    syscall
    ret

x_sys_mmap:
    mov     r10,    rcx
    mov     eax,    0x9 ; sys_mmap
    syscall
    ret

x_sys_mprotect:
    mov     eax,    10  ; sys_mprotect
    syscall
    ret    

x_sys_socket:
    mov     eax,    41
    syscall
    ret

x_sys_connect:
    mov     eax,    42
    syscall
    ret

x_sys_sendto:
    mov     eax,    44
    syscall
    ret

x_sys_recvfrom:
    mov     eax,    45
    syscall
    ret

x_sys_shutdown:
    mov     eax,    48
    syscall
    ret

x_sys_exit:
    mov     eax,    60  ; sys_exit (0x3c)
    syscall
    ret