#ifndef __INTERNAL_H__
#define __INTERNAL_H__


SOCKET getsock(const char *host, unsigned short port, int family, int socktype, int protocol);

#define getsockudp(host,port) getsock(host, port, AF_INET, SOCK_DGRAM, IPPROTO_UDP)
#define getsocktcp(host,port) getsock(host, port, AF_INET, SOCK_STREAM, IPPROTO_TCP)

char *getpacket(SOCKET s, size_t * len);

#endif
