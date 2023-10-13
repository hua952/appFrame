#ifndef sysinc_h__ 
#define sysinc_h__ 

#include <io.h>
#include <stdint.h>
void accept_cb(intptr_t fd, short events, void* arg);
void event_callback_com(intptr_t, short, void *);
#endif
