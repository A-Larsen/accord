#include "parse.h"

char getRandchar(){
	return (char)floor((float)rand() / RAND_MAX * 26) + 'a';
}

void getRandStr(char a[20]){

	int i = 0;

	for(i = 0; i < 20; i++){
		a[i] = getRandchar();
	}

	a[i] = 0;

}

long int 
parseHTML(fp, str)
  FILE *fp;
  char *str;
{
	if(!fp){
		fprintf(stderr, "Could not open file\n");
		return 1;
	}

	rewind(fp);
	fpos_t position;
	fgetpos(fp, &position);

	fseek(fp,0, SEEK_END);
	long int size = ftell(fp);

	fsetpos(fp, &position);

	if(!fread(str, size, 1, fp)){
		fprintf(stderr, "file with descriptor %d failed to read\n", 
				fileno(fp));
		return 0;
	}
	str += size;
	*str++ = 0;

	return size;
}


static int 
mcb(data, cmp, other)
  char *data;
  const char *cmp;
  void *other;
{

	char *dp = data;
	int n = strlen(cmp);

	if(!strncmp(data, cmp, n)){

		data += (n+2);

		/* char tmp[200]; */
		char tmp[400];

		int i = 0;
		for(; *data != '\n'; i++, data++){
			if(i > 30) break;
			tmp[i] = *data;
		}

		tmp[i] = 0;
		MessageData *md = (MessageData *)other;

		if(!strcmp(cmp, "closing")){
			md->closing = !strcmp(tmp, "true");
		}

		if(!strcmp(cmp, "id")){
			md->id = atoi(tmp);
		}

		if(!strcmp(cmp, "date")){
			md->lldate = atoll(tmp);
		}

		if(!strcmp(cmp, "chatroom")){
			md->chatroom = strdup(tmp);
		}

		if(!strcmp(cmp, "initchatroom")){
			md->initchatroom = strdup(tmp);
		}

		if(!strcmp(cmp, "addroom")){
			printf("*ADDING A ROOM !*\n");

			md->roomid = malloc(sizeof(char)*21); 
			getRandStr(md->roomid);
			md->roomalias = strdup(tmp);
		}

		if(!strcmp(cmp, "addfriend")){
			
			/* md->addfriend = strdup(tmp); */
			parseArrayList(tmp, &md->addfriend);
		}
		/* if(!strcmp(cmp, "friendid")){ */
		/* 	md->friendid = strdup(tmp); */
		/* } */

		/* if(!strcmp(cmp, "roomname")){ */
		/* 	md->crname = strdup(tmp); */
		/* } */

	}

	return data - dp;
}

void 
parseMessage(data, md)
  char *data;
  MessageData *md;
{
	printf("%s\n", data);

	int maxheadersize = 400;
	int headersize = 0;

	char message[1000];
	char *mp = message;
	md->chatroom = NULL;
	md->initchatroom = NULL;

	md->roomid = NULL;

	md->lldate = 0;
	/* md->addfriend.room = NULL; */
	md->addfriend.items = NULL;
	/* md->crname = NULL; */

	for(;headersize < maxheadersize; headersize++){

		data += mcb(data, "id",  md);
		data += mcb(data, "closing", md);
		data += mcb(data, "date", md);
		data += mcb(data, "chatroom", md);
		data += mcb(data, "initchatroom", md);
		data += mcb(data, "addroom", md);
		data += mcb(data, "addfriend", md);
		/* data += mcb(data, "friendid", md); */
		/* data += mcb(data, "roomname", md); */

		if(*data == '\n' && *(data+1) == '\n'){
			break;
		}

		data++;
	}


	for(data += 2 ;*data != '\n'; data++){
		*mp++ = *data;
	}


	*mp = 0;

	md->message = strdup(message);
}

int 
parseArrayList(list, cr)
  char *list;
  Darray *cr;
{

	char *arrayp = list;

	char **chatroom = NULL;
	int cidx = 0;

	int arraysize = 0;

	if(strlen(list) > 0){
		for(;*list != 0; list++){

			if(*list == ';'){
				int tmpsize = list - arrayp;
				int move = tmpsize;

				arraysize += move;

				if(!chatroom)
					chatroom = malloc(sizeof(char) * move);
				else
					chatroom = 
						realloc(chatroom, sizeof(char) * arraysize);

				chatroom[cidx] = malloc(sizeof(char) * move);
				strncpy(chatroom[cidx], arrayp, move);
				chatroom[cidx][move] = 0;
				cidx++;

				arrayp += tmpsize+1;
			}
		}
	}

	cr->items = chatroom;
	cr->size = arraysize;
	cr->len = cidx;
	/* cr->chatlen = -1; */

	return 1;
}


char * 
parseToJSONforClient(name, md)
  const char *name;
  MessageData md;
{
		char messagewrap[9999];
		if(md.lldate){
			sprintf(messagewrap, 
					CLIENTJSON, 
					name, 
					md.message, 
					ts_to_readable(md.lldate),
					md.chatroom);
		}else{
			sprintf(messagewrap, 
					CLIENTJSON, 
					name, 
					md.message, 
					md.sdate,
					md.chatroom);
		}

		return strdup(messagewrap);
}

char * arrayToJSONArray(a, len)
  char **a;
  int len;
{

	/* if(len > 10){ */

	/* } */
	/* char js[50] = "["; */
	// 20 is the length of a
	// chatroom table in the database
	// 10 is the maximum number of
	// characters for an alias
	// max number of chatrooms
	// chould be 10
	// 2 for []
	// 3 for "",
	// because the last element isn't ,
	// we can just leave it for the null character
	// maximum characters... 30
	char js[20 * 10 + ((10 * 30)*3) + 2] = "[";
	/* char js[512] = "["; */
	/* char js[500] = "["; */
	char *jsp = js + 1;

	int check = len - 1;
	for(int i = 0; i < len; i++){

		*jsp++ = '\"';

		int len = strlen(a[i]);

		strncpy(jsp, a[i], len);
		jsp += len;
		*jsp++ = '\"';

		if( i < check ){
			*jsp++ = ',';

		}
	}

	*jsp++ = ']';
	*jsp = 0;

	return strdup(js);
}
