#include "parse.h"
#include <libxml/parser.h>

char getRandchar(){
	return (char)floor((float)rand() / (float)RAND_MAX * 26) + 'a';
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
		ONION_ERROR("Could not open file\n");
		return 1;
	}

	rewind(fp);
	fpos_t position;
	fgetpos(fp, &position);

	fseek(fp,0, SEEK_END);
	long int size = ftell(fp);

	fsetpos(fp, &position);

	if(!fread(str, size, 1, fp)){
		ONION_ERROR("file with descriptor %d failed to read\n", 
				fileno(fp));
		return 0;
	}
	str += size;
	*str++ = 0;
	fclose(fp);

	return size;
}

void
xmlPropToData(node, names, vars, len)
	xmlNode *node;
	char **names;
	char ***vars;
	int len;
{
	xmlAttr *attr = node->properties;
	for(int i = 0; i < len; attr = node->properties->next, i++){
		if(!strcmp(names[i], (char *)attr->name)){
			*vars[i] = strdup(
					(char *)xmlGetProp(node, attr->name));
		}
	}
}

void 
parseXML(data, md)
  char *data;
  ClientData *md;

  // not really any bound checking. we can do some bounds checking on the 
  // client side maybe. As long as there is a correct number of properties
  // and nodes there shouldn't be too much of a problem
{

	ONION_INFO("XML----\n%s\n------\n", data);
	md->message.chatroom = NULL;
	md->initchatroom = NULL;

	md->addroom.id = NULL;

	md->message.lldate = 0;

	xmlDoc *doc;
	doc = xmlParseDoc((const xmlChar *)data);

	if(!doc){
		ONION_ERROR("Document not parsed succesfully. \n");
		return;
	}

	xmlNode *node = xmlDocGetRootElement(doc);

	md->user.closing = false;
	md->addfriend.roomname = NULL;

	if(node){
		if(!strcmp((const char *)node->name, "init_room")){
			md->initchatroom = (char *)node->xmlChildrenNode->content;
		}
		else if(!strcmp((const char *)node->name, "addroom")){

			md->addroom.id = malloc(sizeof(char)*21); 
			getRandStr(md->addroom.id);
			md->addroom.alias = strdup(
					(char *)node->xmlChildrenNode->content);
		}
		else if(!strcmp((const char *)node->name, "addfriend")){
			char *names[2] = {"id", "name"};

			char **vars[2] = {&md->addfriend.roomid, &md->addfriend.roomname};

			xmlPropToData(node, names, vars, 2);

			md->addfriend.name = strdup(
					(char *)node->xmlChildrenNode->content);
		}
		else if(!strcmp((const char *)node->name, "root")){
			xmlNode *cur_root;

			for(cur_root = node->children; cur_root; cur_root = cur_root->next){
				if(!strcmp((char *)cur_root->name, "user")){
					char *names[2] = {"closing", "id"};
					char *closing_tmp = NULL;
					char *id_tmp = NULL;
					char **vars[2] =  {&closing_tmp, &id_tmp};

					xmlPropToData(cur_root, names, vars, 2);
					if(closing_tmp) md->user.closing = !strcmp(closing_tmp, "true");

					if(id_tmp) md->user.id = atoi(id_tmp);

					free(closing_tmp);
					free(id_tmp);
				}

				if(!strcmp((char *)cur_root->name, "message")){

					if((char *)cur_root->xmlChildrenNode->content){

						char *names[2] = {"chatroom", "date"};
						char *date_tmp = NULL;
						char **vars[2] = {&md->message.chatroom, &date_tmp};

						xmlPropToData(cur_root, names, vars, 2);

						if(date_tmp) md->message.lldate = atoll(date_tmp);

						md->message.content = strdup(
								(char *)cur_root->xmlChildrenNode->content);
						
						free(date_tmp);
					}else{
						ONION_INFO("message not found\n");
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
  ClientData md;
{
		if(md.message.lldate){
			int len = strlen(CLIENTJSON) +
				strlen(name) + 
				strlen(md.message.content) +
				strlen(md.message.chatroom);

			char *date = ts_to_readable(md.message.lldate);

			len += strlen(date) + 1;

			char messagewrap[len];

			snprintf(messagewrap, 
					len,
					CLIENTJSON, 
					name, 
					md.message.content, 
					ts_to_readable(md.message.lldate),
					md.message.chatroom);

			return strdup(messagewrap);

		}else{
			int len = strlen(CLIENTJSON) +
				strlen(name) + 
				strlen(md.message.content) +
				strlen(md.message.chatroom) +
				strlen(md.message.sdate) + 1;

			char messagewrap[len];

			snprintf(messagewrap, 
					len,
					CLIENTJSON, 
					name, 
					md.message.content, 
					md.message.sdate,
					md.message.chatroom);

			return strdup(messagewrap);
		}

		return NULL;
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
		ONION_ERROR("could not find file: PWADMINS");
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

char *
parseCookie(cookiecpy)
	char *cookiecpy;
{
	if(!strstr(cookiecpy, "="))
		return NULL;

	/* char *c = strdup((char *)cookiecpy); */
	strsep( &cookiecpy, "=");
	
	return cookiecpy;
}
