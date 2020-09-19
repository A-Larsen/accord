#include "parse.h"
#include <libxml/parser.h>

xmlDoc *
parseDoc(char *str){
	xmlDoc *doc;
	doc = xmlParseDoc((const xmlChar *)str);

	if(doc == NULL){
		fprintf(stderr, "Document not parsed succesfully. \n");
		return NULL;
	}

	return doc;
}

xmlNode *
getNode(xmlNode *start, xmlChar *nodename){
	xmlNode *node;

	for(node = start->children; node; node = node->next){
			if(!xmlStrcmp(node->name, nodename)){
				return node;
			}
	}

	return NULL;
}

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
	fclose(fp);

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
			parseArrayList(tmp, &md->addfriend);
		}
	}

	return data - dp;
}

void 
parseMessage(data, md)
  char *data;
  MessageData *md;
{

	/* printf("%s\n", data); */
	md->chatroom = NULL;
	md->initchatroom = NULL;

	md->roomid = NULL;

	md->lldate = 0;
	/* md->addfriend.items = NULL; */
	/* md->addfriend = NULL; */

	xmlDocPtr doc = parseDoc(data);

	xmlNode *node = xmlDocGetRootElement(doc);
	md->closing = false;
	md->addfriend.roomname = NULL;

	if(node){
		if(!strcmp((const char *)node->name, "init_room")){
			char *str = (char *)node->xmlChildrenNode->content;
			md->initchatroom = str;
		}
		else if(!strcmp((const char *)node->name, "addroom")){
			char *str = (char *)node->xmlChildrenNode->content;
			md->initchatroom = str;

			md->roomid = malloc(sizeof(char)*21); 
			getRandStr(md->roomid);
			md->roomalias = strdup(str);
		}
		else if(!strcmp((const char *)node->name, "init_room")){
			char *str = (char *)node->xmlChildrenNode->content;
			md->initchatroom = str;
		}
		else if(!strcmp((const char *)node->name, "addfriend")){
			xmlAttr *attr = node->properties;

			/* md->addfriend.items = (char **)malloc(100); */

			/* char *str0 = (char *)xmlGetProp(node, attr->name); */
			/* md->addfriend.items[0] = malloc(strlen(str0)); */
			/* md->addfriend.items[0] = (char *)xmlGetProp(node, attr->name); */
			md->addfriend.name = strdup((char *)xmlGetProp(node, attr->name));

			attr = node->properties->next;
			/* char *str1 = (char *)xmlGetProp(node, attr->name); */
			/* md->addfriend.items[1] = malloc(strlen(str1)); */
			/* md->addfriend.items[1] = (char *)xmlGetProp(node, attr->name); */
			md->addfriend.roomid = strdup((char *)xmlGetProp(node, attr->name));

			attr = node->properties->next;
			/* char *str2 = (char *)xmlGetProp(node, attr->name); */
			/* md->addfriend.items[2] = malloc(strlen(str2)); */
			/* md->addfriend.items[2] = (char *)xmlGetProp(node, attr->name); */
			/* printf("found addfriend\n"); */
			/* char *str = (char *)node->xmlChildrenNode->content; */
			/* parseArrayList(str, &md->addfriend); */
			md->addfriend.roomname = strdup((char *)xmlGetProp(node, attr->name));
		}
		else if(!strcmp((const char *)node->name, "root")){
			xmlNode *cur_root;

			for(cur_root = node->children; cur_root; cur_root = cur_root->next){
				if(!xmlStrcmp(cur_root->name, (const xmlChar *)"user")){
					xmlNode *cur_user;

					for(cur_user = cur_root->children; cur_user;cur_user = cur_user->next){
						if(!xmlStrcmp(cur_user->name, (const xmlChar *)"id")){ 
							char *str = (char *)cur_user->xmlChildrenNode->content;
							printf("id: %s\n", str);
							md->id = atoi((char *)str);
						}
					}
				}

				if(!xmlStrcmp(cur_root->name, (const xmlChar *)"mi")){
					xmlNode *cur_mi;

					for(cur_mi = cur_root->children; cur_mi;cur_mi = cur_mi->next){
						if(!xmlStrcmp(cur_mi->name, (const xmlChar *)"chatroom")){ 
							char *str = (char *)cur_mi->xmlChildrenNode->content;
							md->chatroom = strdup(str);
							printf("chatroom: %s\n", md->chatroom);
						}
						if(!xmlStrcmp(cur_mi->name, (const xmlChar *)"date")){ 
							char *str = (char *)cur_mi->xmlChildrenNode->content;
							printf("date: %s\n", str);
							md->lldate = atoll(str);
						}
						if(!xmlStrcmp(cur_mi->name, (const xmlChar *)"message")){ 
							char *str = (char *)cur_mi->xmlChildrenNode->content;
							printf("message: %s\n", str);
							md->message = strdup(str);
						}
						if(!xmlStrcmp(cur_mi->name, (const xmlChar *)"closing")){ 
							char *str = (char *)cur_mi->xmlChildrenNode->content;
							md->closing = !strcmp(str, "true");
							printf("closing: %d\n", md->closing);
						}
					}
				}
			}
		}
	}
	/* printf("%s\n", data); */

	/* int maxheadersize = 400; */
	/* int headersize = 0; */

	/* char message[1000]; */
	/* char *mp = message; */
	/* md->chatroom = NULL; */
	/* md->initchatroom = NULL; */

	/* md->roomid = NULL; */

	/* md->lldate = 0; */
	/* md->addfriend.items = NULL; */

	/* for(;headersize < maxheadersize; headersize++){ */

	/* 	data += mcb(data, "id",  md); */
	/* 	data += mcb(data, "closing", md); */
	/* 	data += mcb(data, "date", md); */
	/* 	data += mcb(data, "chatroom", md); */
	/* 	data += mcb(data, "initchatroom", md); */
	/* 	data += mcb(data, "addroom", md); */
	/* 	data += mcb(data, "addfriend", md); */

	/* 	if(*data == '\n' && *(data+1) == '\n'){ */
	/* 		break; */
	/* 	} */

	/* 	data++; */
	/* } */


	/* for(data += 2 ;*data != '\n'; data++){ */
	/* 	*mp++ = *data; */
	/* } */


	/* *mp = 0; */

	/* md->message = strdup(message); */
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
