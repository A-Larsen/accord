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

int initDBs();
int findUser(const char *name, const char *password, Chatroom *cr);
int initLogin(Chatroom *cr, onion_websocket *ws);
int storeMessage(Chatroom *cr, const char *name, char *msg, long int date);
void dbfree();
Clist getChatroomUsers(char *current);

int addChatroom(char *id, char *alias, char *name);
int insertIntoChatroomUsers(char *id, char *name);
int updateUsersChatrooms(char *chatrooms, char * name,char * id, char *alias);
char * selectChatroomFromUsername(char *name);

int createUser(const char *name, const char *password);
bool checkIfUsersExists(char *name);
bool checkUsersInChatroom(char *current, char *check);
  /* char *chatrooms; */
  /* char *name; */
  /* char *id; */
  /* char *alias; */
#endif
