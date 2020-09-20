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
#include <errno.h>

#include "../parse.h"
#include "../sort.h"
#include "../mem.h"
#include "../options.h"
#include "database.h"

#define MAX_FILESIZE 0xffff
#define MAXUSERS 4


typedef struct _FileData{
	char *name;
	char *data;
	long int length;
}FileData;


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
