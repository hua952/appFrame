#ifndef _sysinc_h__
#define _sysinc_h__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void accept_cb(int fd, short events, void* arg);
void event_callback_com(int, short, void *);

#endif
