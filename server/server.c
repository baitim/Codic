/*          ,.  ,.
            ||  ||
           ,''--''.
          : (.)(.) :
         ,'        `.
         :          :
         :          :
         `._m____m_,'
*/
#include <stdio.h>

#include "sockets.h"
#include "logger.h"

int main()
{
    logger_ctor();

    add_log_handler((struct Log_handler) { stderr, WARN, 1 });
    FILE *log = fopen("logs/log.txt", "w");
    setvbuf(log, NULL, _IONBF, 0);
    
    if (log)
        add_log_handler((struct Log_handler) { log, DEBUG, 0 });

    uv_loop_t* loop = uv_default_loop();
    struct sockaddr_in addr;
    uv_timer_t timer;

    uv_udp_init(loop, &server);

    log_message(INFO, "Initialized libuv's event loop\n");
    
	uv_ip4_addr("0.0.0.0", 5002, &addr);
    uv_udp_bind(&server, (const struct sockaddr*)&addr, 0);
	uv_udp_recv_start(&server, alloc_buffer, on_recv); //on_recv is subject to change

    log_message(INFO, "Set up receiving callbacks\n");
	
	uv_timer_init(loop, &timer);
	uv_timer_start(&timer, update_state, 1000, 1000);

    log_message(INFO, "Set up state updates\n");

	int result = uv_run(loop, UV_RUN_DEFAULT);

    log_message(INFO, "Stopping...\n");

    logger_dtor();

	return result;
}
