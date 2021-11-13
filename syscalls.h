#ifndef _X0LIB_SYSCALLS_H_
#define _X0LIB_SYSCALLS_H_

#include "socketdef.h"

#define STDIN_FILENO    0
#define STDOUT_FILENO   1
#define STDERR_FILENO   2

extern volatile uint64 x_sys_write(int fd, const char *str, size_t slen);

extern volatile ssize_t x_sys_read(int fd, void *outbuf, size_t count);

extern volatile int x_sys_close(int fd);

extern volatile int x_sys_mprotect(unsigned long start, size_t len, unsigned long prot);

extern volatile int x_sys_socket(int domain, int type, int protocol);

extern volatile int x_sys_connect(int sockfd, const struct sockaddr *uservaddr, uint addrlen);

extern volatile ssize_t x_sys_sendto(int sockfd, const void *buf, size_t len, uint flags, const struct sockaddr *dest_addr, uint addrlen);

extern volatile ssize_t x_sys_recvfrom(int sockfd, void *restrict buf, size_t len, int flags, struct sockaddr *restrict src_addr, uint *restrict addrlen);

extern volatile int x_sys_shutdown(int fd, int how);

extern volatile void x_sys_exit(int return_code);

#endif