#ifndef _SOCKETDEF_H_
#define _SOCKETDEF_H_

#include "types.h"

/**
 * @brief This is all chaotic pasta from glibc source
 */

#define AF_INET		2

/* Types of sockets.  */
enum __socket_type
{
  SOCK_STREAM = 1,		/* Sequenced, reliable, connection-based
				   byte streams.  */
#define SOCK_STREAM SOCK_STREAM
  SOCK_DGRAM = 2,		/* Connectionless, unreliable datagrams
				   of fixed maximum length.  */
#define SOCK_DGRAM SOCK_DGRAM
  SOCK_RAW = 3,			/* Raw protocol interface.  */
#define SOCK_RAW SOCK_RAW
  SOCK_RDM = 4,			/* Reliably-delivered messages.  */
#define SOCK_RDM SOCK_RDM
  SOCK_SEQPACKET = 5,		/* Sequenced, reliable, connection-based,
				   datagrams of fixed maximum length.  */
#define SOCK_SEQPACKET SOCK_SEQPACKET
  SOCK_DCCP = 6,		/* Datagram Congestion Control Protocol.  */
#define SOCK_DCCP SOCK_DCCP
  SOCK_PACKET = 10,		/* Linux specific way of getting packets
				   at the dev level.  For writing rarp and
				   other similar things on the user level. */
#define SOCK_PACKET SOCK_PACKET

  /* Flags to be ORed into the type parameter of socket and socketpair and
     used for the flags parameter of paccept.  */

  SOCK_CLOEXEC = 02000000,	/* Atomically set close-on-exec flag for the
				   new descriptor(s).  */
#define SOCK_CLOEXEC SOCK_CLOEXEC
  SOCK_NONBLOCK = 00004000	/* Atomically mark descriptor(s) as
				   non-blocking.  */
#define SOCK_NONBLOCK SOCK_NONBLOCK
};

/* The following constants should be used for the second parameter of
   `shutdown'.  */
enum
{
  SHUT_RD = 0,		/* No more receptions.  */
#define SHUT_RD		SHUT_RD
  SHUT_WR,		/* No more transmissions.  */
#define SHUT_WR		SHUT_WR
  SHUT_RDWR		/* No more receptions or transmissions.  */
#define SHUT_RDWR	SHUT_RDWR
};

/* POSIX.1g specifies this type name for the `sa_family' member.  */
typedef unsigned short int sa_family_t;
#define	__SOCKADDR_COMMON(sa_prefix) \
  sa_family_t sa_prefix##family

struct in_addr {
    uint s_addr;
};

struct sockaddr {
    __SOCKADDR_COMMON (sa_);
    char sa_data[14];   /* Address data. */
};

struct sockaddr_in {
    __SOCKADDR_COMMON (sin_);
    uint16 sin_port;
    struct in_addr sin_addr;		/* Internet address.  */

    /* Pad to size of `struct sockaddr'.  */
    byte sin_zero[sizeof (struct sockaddr)
            - sizeof(unsigned short int)
            - sizeof(uint16)
            - sizeof( struct in_addr)];
};

#endif