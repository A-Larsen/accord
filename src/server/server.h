#ifndef __SERVER_
#define __SERVER_
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <signal.h>
#include <onion/log.h>
#include <onion/onion.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <sqlite3.h>

/* #include "../options.h" */
#include "../parse.h"
#include "../sort.h"
#include "../mem.h"
#include "database.h"

#define MAX_FILESIZE 0xffff
#define MAXUSERS 4

/* char * FLAGS; */

typedef struct _Data{
	char *name;
	char *data;
	long int length;
	/* FILE *fp; */
}Data;


typedef struct _ConnectedUsers{
	int *indices;
	int len;
} ConnectedUsers;

typedef struct _User{
	int id;
	char name[10];
	onion_websocket *ws;
	bool active;
	Chatrooms chatroom;
	ConnectedUsers cu;
} User;


// extern means declare this latter
extern char OPTIONS;
#define AUTOLOGIN  1 << 0

void server_free();

char *server_get_view(char *name);

int server_init(onion_url *urls);

void server_add_static_file(onion_url *urls, char *routes, char *path);

onion_connection_status server_connection_chat(void *data, onion_request * req,
                                          onion_response * res);

onion_connection_status 
server_connection_home(void *data, onion_request * req,
                                          onion_response * res);

onion_connection_status 
server_connection_login(void *_, onion_request * req, onion_response * res);

onion_connection_status 
server_connection_signup(void *_, onion_request *req, onion_response *res);

onion_connection_status 
server_connection_sitemap(void *_, onion_request *req, onion_response *res);
#endif
