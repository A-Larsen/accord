#ifndef __PARSE__
#define __PARSE__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <string.h>
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

typedef struct _MessageData{
	char *message;
	long long int lldate;
	char *sdate;
	int id;
	bool closing;
	char *chatroom;
	char *initchatroom;
	char *roomid;
	char *roomalias;
	/* Darray addfriend; */
	Addfriend addfriend;

} MessageData;


int parseArrayList(char *list, Darray *cr);
void parseMessage(char *data, MessageData *md);
long int parseHTML(FILE *fp, char *str);
void nsort(void **data, int len);
char * parseToJSONforClient(const char *name, MessageData md);
char * arrayToJSONArray(char **a, int len);

#endif
