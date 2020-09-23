#ifndef __PARSE__
#define __PARSE__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <bsd/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "convert.h"

#define CLIENTJSON "{"\
						"\"name\": \"%s\", "\
						"\"message\": \"%s\", "\
						"\"date\": \"%s\", "\
						"\"chatroom\": \"%s\" "\
				  	"}"

typedef struct _Darray {
	char **items;
	long int size;
	int len;
}Darray;

typedef struct _Chatrooms {
	Darray rooms;
	char *current;
	long long int chatlen;
}Chatrooms;

typedef struct _Addfriend{
	char *name;
	char *roomid;
	char *roomname;
}Addfriend;

typedef struct _AddRoom{
	char *id;
	char *alias;
}AddRoom;

typedef struct _Message{
	char *content;
	char *chatroom;
	long long int lldate;
	char *sdate;
}Message;

typedef struct _MUser{
	int id;
	bool closing;
}MUser;

typedef struct _ClientData{
	MUser user;
	char *initchatroom;
	Message message;
	AddRoom addroom;
	Addfriend addfriend;

} ClientData;


char * admin_search(const char * admin);
int parseArrayList(char *list, Darray *cr);
void parseXML(char *data, ClientData *md);
long int fileToStr(FILE *fp, char *str);
void nsort(void **data, int len);
char * parseToJSONforClient(const char *name, ClientData md);
char * arrayToJSONArray(char **a, int len);
char * parseCookie(const char *cookie);

#endif
