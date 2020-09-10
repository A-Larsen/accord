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
	/* char **room; */
	char **items;
	long int size;
	int len;
	/* long long int chatlen; */
	/* char current[15]; */
	/* char *current; */
}Darray;

typedef struct _Chatrooms {
	Darray rooms;
	/* char **room; */
	/* long int size; */
	/* int len; */
	/* long long int chatlen; */
	/* char current[15]; */
	char *current;
	long long int chatlen;
}Chatrooms;

typedef struct _MessageData{
	char *message;
	long long int lldate;
	char *sdate;
	int id;
	bool closing;
	char *chatroom;
	char *initchatroom;
	/* char *addroom; */
	/* long long int uuid */
	/* uuid_t uuid; */
	char *roomid;
	char *roomalias;
	/* char *addfriend; */
	/* char **addfriend; */
	Darray addfriend;
	/* char *friendid; */
	/* char *crname; */

} MessageData;

int parseArrayList(char *list, Darray *cr);
void parseMessage(char *data, MessageData *md);
long int parseHTML(FILE *fp, char *str);
void nsort(void **data, int len);
char * parseToJSONforClient(const char *name, MessageData md);
char * arrayToJSONArray(char **a, int len);

#endif
