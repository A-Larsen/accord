#include "parse.h"
#include <libxml/parser.h>

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
fileToStr(fp, str)
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

void 
parseXML(data, md)
  char *data;
  MessageData *md;
{

	printf("XML----\n%s\n------\n", data);
	md->chatroom = NULL;
	md->initchatroom = NULL;

	md->roomid = NULL;

	md->lldate = 0;

	xmlDoc *doc;
	doc = xmlParseDoc((const xmlChar *)data);

	if(!doc){
		fprintf(stderr, "Document not parsed succesfully. \n");
		return;
	}

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
		else if(!strcmp((const char *)node->name, "addfriend")){
			char *str = (char *)node->xmlChildrenNode->content;
			xmlAttr *attr = node->properties;

			md->addfriend.roomid = strdup((char *)xmlGetProp(node, attr->name));

			attr = node->properties->next;
			md->addfriend.roomname = strdup((char *)xmlGetProp(node, attr->name));

			if(str)
				md->addfriend.name = strdup(str);
		}
		else if(!strcmp((const char *)node->name, "root")){
			xmlNode *cur_root;

			for(cur_root = node->children; cur_root; cur_root = cur_root->next){
				if(!xmlStrcmp(cur_root->name, (const xmlChar *)"user")){
					xmlAttr *attr = cur_root->properties;
					md->closing = !strcmp((char *)xmlGetProp(cur_root, attr->name), "true");
					attr = cur_root->properties->next;
					md->id = atoi((char *)xmlGetProp(cur_root, attr->name));
				}

				if(!xmlStrcmp(cur_root->name, (const xmlChar *)"message")){

					char *str = (char *)cur_root->xmlChildrenNode->content;

					if(str){
						xmlAttr *attr = cur_root->properties;
						md->chatroom = strdup((char *)xmlGetProp(cur_root, attr->name));

						attr = cur_root->properties->next;

						md->lldate = atoll((char *)xmlGetProp(cur_root, attr->name));

						printf("message: %s\n", str);
						md->message = strdup(str);
					}
				}
			}
		}
	}
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
				/* strncpy(chatroom[cidx], arrayp, move); */
				strlcpy(chatroom[cidx], arrayp, move+1);
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

		/* strncpy(jsp, a[i], len); */
		strlcpy(jsp, a[i], len + 1);
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

char * 
admin_search(admin)
  const char *admin;
{
	FILE *fp = fopen("PWADMINS", "r");

	if(!fp){
		fprintf(stderr, "could not find file: PWADMINS");
	}

	int ch = 0;

	while(1){

		ch = fgetc(fp);

		while(ch == '\t' || ch == ' ') ch = fgetc(fp);

		if(ch == EOF){
			break;
		}

		char name[10];
		char *np = name;

		char password[31] = {};
		char *pp = password;

		if(ch == '['){
			ch = fgetc(fp);
			while(ch != ']'){
				 *np++ = ch;
				 ch = fgetc(fp);
			}

			*np++ = 0;

			ch = fgetc(fp);

			if(!strcmp(name, admin)){
				while(ch == '\n') ch = fgetc(fp);

				while(ch != '\n'){
					 *pp++ = ch;
					 ch = fgetc(fp);
				}

				*pp++ = 0;

				return strdup(password);
			}
		}
	}

	return NULL;
}
