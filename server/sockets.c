#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "user.h"
#include "sockets.h"

#define MAX_SIZE_PROG_BUFFER 10000

struct _User {
	int id;
	struct sockaddr_in addr;
	User_status status;
	int score;
};

#define MAX_COUNT_USERS 100000
User users[MAX_COUNT_USERS];
uv_udp_t server;

void register_user		(const struct sockaddr* addr);
void parse_program		(const uv_buf_t* buf, const struct sockaddr* addr);
void test_program		(User *user, char* program_text);
void run_program		(char *program_text);
void send_state			();
void send_buffer		(User* user, char* buf, size_t sz);
void req_delete			(uv_udp_send_t* req, int status);
static char* skip_spaces(char* str);

void alloc_buffer(uv_handle_t* user, size_t sz, uv_buf_t* buf) 
{
	buf->base = malloc(sz);
	buf->len  = sz;
}

//TODO: change to json
void on_recv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, 
             const struct sockaddr* addr, unsigned flags)
{
	if (nread > 0) {
		printf("%ld bytes got from user\n", nread);
		if (strncmp(buf->base, "CONNECT", 7) == 0) {
			printf("Registering...\n");
			register_user(addr);
		} else if (nread > 7 && strncmp(buf->base, "PROGRAM", 7) == 0) {
			parse_program(buf, addr);
		} else { 
			printf("Unknown request\n");
		}
	}
}

void register_user(const struct sockaddr* addr)
{
	for (int i = 0; i < MAX_COUNT_USERS; i++ ) {
		if (users[i].status == USER_STATUS_INACTIVE) {
			struct sockaddr_in* addr_in = (struct sockaddr_in*)addr;
			char* a = (char*)&(addr_in->sin_addr.s_addr);
			printf("Registering user from %d.%d.%d.%d:%d\n", a[0], a[1], a[2], a[3], addr_in->sin_port);
			users[i] = (User){i, *addr_in, USER_STATUS_ACTIVE, 0};
			return;
		}
	}
}

//TODO: add user registration and authorization
void parse_program(const uv_buf_t* buf, const struct sockaddr* addr)
{
	int id = 0;
	int nread = 0;
	if (sscanf(buf->base, "PROGRAM %d%n", &id, &nread) == 1) {
		printf("Message got from %d\n", id);

		for (int i = 0; i < MAX_COUNT_USERS; i++) {
			if (users[i].status == USER_STATUS_INACTIVE) continue;

			if (users[i].id == id && memcmp(&users[i].addr, addr, sizeof(struct sockaddr_in) != 0)) {
				fprintf(stderr, "Error: unauthorized access!\n");
				return;
			} 
			else if (users[i].id == id) {
				test_program(users + i, buf->base + nread);
				return;
			}
		}
	} else {
		fprintf(stderr, "Error while parsing program: message format invalid\n");
	}
}

void test_program(User *user, char* program_text)
{   
    clock_t start = clock();
    run_program(program_text);
    clock_t end = clock();
    clock_t execution_ticks = end - start;
    double seconds = (double) execution_ticks / CLOCKS_PER_SEC;
    printf("Execution time of %d: %lf\n", user->id, seconds);

	int sz = 0;
	char buf[MAX_SIZE_PROG_BUFFER];
	sz += sprintf(buf, "TIME %lf\n", seconds);
	send_buffer(user, buf, sz);
}

void update_state(uv_timer_t* timer)
{
	send_state();
}

void send_state()
{
	size_t sz = 0;
	User* user;

	char state_buf[MAX_SIZE_PROG_BUFFER];
	for (user = users; user < users + MAX_COUNT_USERS; user++) {
		if (user->status == USER_STATUS_INACTIVE) continue;
		sz += sprintf(state_buf + sz, "STATE %d %d %d\n", user->id, user->status, user->score);
		printf("User %d: %d %d\n", user->id, user->status, user->score);
	}

	for (user = users; user < users + MAX_COUNT_USERS; user++) {
		if (user->status == USER_STATUS_INACTIVE) continue;
		send_buffer(user, state_buf, sz);
	}
}

void send_buffer(User* user, char* buf, size_t sz)
{
	uv_buf_t wbuf = uv_buf_init(buf, sz);
	uv_udp_send_t* req = malloc(sizeof(uv_udp_send_t));
	uv_req_set_data((uv_req_t*)req, buf);
	uv_udp_send(req, &server, &wbuf, 1, (const struct sockaddr*)&user->addr,
			req_delete);
}

void req_delete(uv_udp_send_t* req, int status)
{
	free(req);
}
