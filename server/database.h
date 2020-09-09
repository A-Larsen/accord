#ifndef __DATABASE__
#define __DATABASE__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sqlite3.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <onion/websocket.h>
#include <time.h>

#include "../parse.h"
#include "../convert.h"
#include "../mem.h"

typedef struct _Clist{
	char **list;
	int len;
}Clist;

int db_init();
int db_find_user(const char *name, const char *password, Chatroom *cr);
int db_init_login(Chatroom *cr, onion_websocket *ws);
int db_store_message(Chatroom *cr, const char *name, char *msg, long int date);
void db_free();
Clist db_get_chatroom_users(char *current);

int db_add_chatroom(char *id, char *alias, char *name);
int db_insert_into_chatroom_users(char *id, char *name);
int db_update_users_chatrooms(char *chatrooms, char * name,char * id, char *alias);
char * db_select_chatroom(char *name);

int db_create_user(const char *name, const char *password);
bool db_check_if_user_exists(char *name);
/* bool db_check_if_user_in_chatroom(char *current, char *check); */
  /* char *chatrooms; */
  /* char *name; */
  /* char *id; */
  /* char *alias; */
#endif
