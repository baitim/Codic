#ifndef SERVER_SOCKETS_H
#define SERVER_SOCKETS_H

#include <stdio.h>

#include "uv.h"

extern uv_udp_t server;

typedef struct _User User;

void alloc_buffer   (uv_handle_t* client, size_t sz, uv_buf_t* buf);
void on_recv        (uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, 
                     const struct sockaddr* addr, unsigned flags);
void update_state   (uv_timer_t* timer);

#endif // SERVER_SOCKETS_H
