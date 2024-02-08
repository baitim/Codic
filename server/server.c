/*          ,.  ,.
            ||  ||
           ,''--''.
          : (.)(.) :
         ,'        `.
         :          :
         :          :
         `._m____m_,'
*/

#include "sockets.h"

int main()
{
    uv_loop_t* loop = uv_default_loop();
    struct sockaddr_in addr;
    uv_timer_t timer;

    uv_udp_init(loop, &server);
    
	uv_ip4_addr("213.226.124.209", 5002, &addr);
    uv_udp_bind(&server, (const struct sockaddr*)&addr, 0);
	uv_udp_recv_start(&server, alloc_buffer, on_recv); //on_recv is subject to change
	
	uv_timer_init(loop, &timer);
	uv_timer_start(&timer, update_state, 100, 100);

	int result = uv_run(loop, UV_RUN_DEFAULT);

	return result;
}