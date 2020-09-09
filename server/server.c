#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "server.h"

static bool CLOSING        = false;
static User *closinguser   = NULL;

static bool FOUNDUSER      = false;
static User **USERS;

static int VIEWSN          = 0;

static int PUBLICN         = 0;

// zero indexed count
static int usercount       = -1;

static Data VIEWS         [10];
static Data PUBLIC        [10];

char *
server_get_view(name)
  char *name;
{

	for(int i = 0; i < VIEWSN; i++){
		if(!strcmp(VIEWS[i].name, name)){
			return VIEWS[i].data;
		}
	}

	return NULL;
}

void 
server_add_dir(urls, dir)
  onion_url *urls;
  char *dir;
{
	struct dirent *de;
	DIR *dr = opendir(dir);

	if(!dr){
		fprintf(stderr, "Could not open views directory");
	}
	
	int n = 0;

	FILE *fp;
	char tmp[MAX_FILESIZE];

	while((de = readdir(dr))){

		if(strcmp(de->d_name, ".") && strcmp(de->d_name, "..")){
			
			char *rootdir = strdup(dir);

			fp = fopen(strcat(rootdir, de->d_name), "r");
			ONION_INFO("fileno: %d", fileno(fp));
			long int len;
			len = parseHTML(fp, tmp);

			Data data;
			data.name = strdup(de->d_name);
			data.data = strdup(tmp);
			data.length = len;
			data.fp = fp;

			if(!strcmp(dir, "views/")){
				memcpy(&VIEWS[n], &data, sizeof(data));
				VIEWSN++;
			}
			else if(!strcmp(dir, "public/")){
				memcpy(&PUBLIC[n], &data, sizeof(data));
				addStaticFile(urls, PUBLIC[n].name);
				PUBLICN++;
			}

			n++;
			free(rootdir);
		}
	}

	closedir(dr);
}

int 
server_create_user(name, cr)
  char *name;
  Chatroom cr;
{
	ONION_INFO("creating users '%s'", name);

	for(int i = 0; i <= usercount; i++){
		if(USERS[i] && !strcmp(USERS[i]->name, name)){
			USERS[i] = NULL;
		}
	}

	sortn((void **)USERS, usercount+1);

	usercount++;
	/* USERS = (User **)dynamicArrayResize((void **)USERS, usercount + 1, sizeof(User)); */

	ONION_INFO("*RESIZING USERS ARRAY*");
	USERS = (User **)dynamicArrayResize((void **)USERS, usercount + 1, sizeof(User));

	USERS[usercount]->id = -1;
	USERS[usercount]->ws = NULL;

	size_t len = strlen(name);
	strncpy(USERS[usercount]->name, name, len);
	USERS[usercount]->name[len] = 0;

	USERS[usercount]->chatroom = cr;
	USERS[usercount]->chatroom.current = NULL;

	USERS[usercount]->active = false; 
	return 1;
}

int 
init_Server(urls)
  onion_url *urls;
{
	srand(time(0));
	int rc;
	server_add_dir(NULL, "views/");
	server_add_dir(urls, "public/");
	rc = initDBs();

	return rc;
}

void 
addStaticFile(urls, name)
  onion_url *urls;
  char *name;
{
	size_t len = strlen(name) + 8;
	char file[len];
	snprintf(file, len, "public/%s", name);
	FILE *fp = fopen(file, "r");

	char filestr[MAX_FILESIZE];
	parseHTML(fp, filestr);

	onion_url_add_static(urls, name, filestr, HTTP_OK);
}


User *
getUser(id)
  int id;
{
	for(int i = 0; i <= usercount; i++){
		if(USERS[i] && USERS[i]->id == id){
			return USERS[i];
		}
	}
	return NULL;
}

void 
server_websocket_printf_all(message)
  char *message;
{
	if(usercount+1 > 0){
		for(int i = 0; i <= usercount; i++){
			if(USERS[i] && strcmp(message, "")){
				onion_websocket_printf(USERS[i]->ws, message);
			}
		}
	}
}

void 
server_websocket_printf_connected(user,  message)
  User *user;
  char *message;
{
	if(usercount+1 > 0){
		for(int i = 0; i < user->cu.len; i++){
			if(USERS[user->cu.indices[i]] && !strcmp(USERS[user->cu.indices[i]]->chatroom.current, user->chatroom.current))
				onion_websocket_printf(USERS[user->cu.indices[i]]->ws, message);
		}
	}

}

void server_websocket_get_connectedUsers(user, exclude)
  User *user; 
  char *exclude;
{
	ConnectedUsers cu;
	/* cu.len = 0; */

	Clist c = getChatroomUsers(user->chatroom.current);
	char *a[usercount+1];
	int len = 0;


	for(int i = 0; i < c.len; i++){
		for(int j = 0; j <= usercount; j++){
			if(USERS[j] && !strcmp(USERS[j]->name, c.list[i]) && strcmp(USERS[j]->name, exclude)){

				cu.indices = (int *)dynamicArrayResize2(cu.indices, len + 1, sizeof(int));

				cu.indices[len] = j;
				/* cu.len++; */
				len++;
				a[i] = strdup(USERS[j]->name);
			}
		}
	}

	cu.len = len;
	char *tmp = NULL;
	char *ja = arrayToJSONArray(a, len);

	if(ja && len > 0)
		asprintf(&tmp, "{\"friends\":%s}", ja);

	printf("SENDING %s\n", tmp);

	for(int i = 0; i < c.len; i++){
		for(int j = 0; j <= usercount; j++){
			/* if(USERS[j] && !strcmp(USERS[j]->name, c.list[i])){ */
			if(USERS[j] && !strcmp(USERS[j]->name, c.list[i]) && strcmp(USERS[j]->name, exclude)){
				USERS[j]->cu = cu;
				if(tmp){
					onion_websocket_printf(USERS[j]->ws, tmp);
				}
			}
		}
	}

	free(tmp);
	free(ja);
}


int 
closeUser(user)
  User *user;
{
	/* User *usercheck = malloc(sizeof(User)); */
	/* memcpy(usercheck, user, sizeof(User)); */

	if(!user)
		return 0;

	// problems occur when closing in a a different order
	// than opening when using this function below
	/* server_websocket_get_connectedUsers(user, user->name); */

	onion_websocket_close(user->ws, "1001");
	printf("closing: %s %d\n", user->name, user->id);

	for(int i = 0; i <= usercount; i++){
		if(USERS[i] && USERS[i]->name == user->name){
			printf("setting user with name %s to NULL\n", USERS[i]->name);
			USERS[i] = NULL;
		}
	}

	sortn((void **)USERS, usercount+1);

	usercount--;
	CLOSING = false;

	return 1;
	/* server_websocket_get_connectedUsers(usercheck, usercheck->name); */
}

void
server_websocket_send_chatrooms(user)
  User *user;
{
	if(user->chatroom.len == 0)
		return;

	char *tmp = NULL;

	char *a = arrayToJSONArray(user->chatroom.room, user->chatroom.len);

	if(a){
		asprintf(&tmp, "{\"initchatrooms\":%s}", a);
		printf("JSON ROOMS: %s\n", tmp);
		onion_websocket_printf(user->ws, tmp);
		free(tmp);
		free(a);
	}

}

void 
server_add_friend(user, md)
  User *user;
  MessageData md;
{
	printf("ADDING A FRINED: %s\n", md.addfriend);
	printf("ADDING A FRINED ID: %s\n", md.friendid);
	if(checkIfUsersExists(md.addfriend)){
		insertIntoChatroomUsers(md.friendid, md.addfriend);
		char * chatrooms = selectChatroomFromUsername(md.addfriend);
		updateUsersChatrooms(chatrooms, md.addfriend, md.friendid, md.crname);
	}else{
		onion_websocket_printf(user->ws, "{\"error\": \"user does not exist\"}");
	}

}

void
server_load_chatroom(user, md)
  User *user;
  MessageData md;
{
	ONION_INFO("INIT CHATROOM: %s", md.initchatroom);
	user->chatroom.current = strdup(md.initchatroom);
	server_websocket_get_connectedUsers(user, "");
	initLogin(&user->chatroom, user->ws);
}

void 
server_send_message(user, md)
  User *user;
  MessageData md;
{
	user->chatroom.current = strdup(md.chatroom);
	char *message = parseToJSONforClient(user->name, md);

	server_websocket_printf_connected(user, message);
	storeMessage(&user->chatroom,
				user->name, 
				md.message, 
				md.lldate);
}

onion_connection_status 
server_websocket_chat(data, ws, data_ready_len) 
  void *data;
  onion_websocket *ws;
  ssize_t data_ready_len;
{

	if(CLOSING && closinguser){
		closeUser(closinguser);
		return OCS_CLOSE_CONNECTION;
	}

	char tmp[256];
	if ((long unsigned int)data_ready_len > sizeof(tmp))
		data_ready_len = sizeof(tmp) - 1;

	int len = onion_websocket_read(ws, tmp, data_ready_len);

	if (len <= 0) {
		ONION_ERROR("Error reading data: %d: %s (%d)", errno, strerror(errno),
					data_ready_len);
		return OCS_NEED_MORE_DATA;
	}
	tmp[len] = 0;

	MessageData md;
	parseMessage(tmp, &md);

	User *user = getUser(md.id);

	if(USERS[usercount] && USERS[usercount]->id == -1){
		USERS[usercount]->id = md.id;

		if(USERS[usercount]->chatroom.len > 0)
			server_websocket_send_chatrooms(USERS[usercount]);

		return OCS_NEED_MORE_DATA;
	}
	
	if(user){
		if(md.closing){
			closinguser = user;
			CLOSING = true;
			return OCS_NEED_MORE_DATA;
		}

		else if(md.addfriend && md.friendid && md.crname){
			server_add_friend(user, md);
		}

		else if(md.roomid){
			printf("ROOMID: %s\n", md.roomid);
			printf("ROOM ALIAS: %s\n", md.roomalias);
			addChatroom(md.roomid, md.roomalias, user->name);
			onion_websocket_printf(user->ws, "{\"reload\": \"/login\"}");
		}

		else if(md.initchatroom){
			server_load_chatroom(user, md);
		}

		else if(md.chatroom){
			server_send_message(user, md);
		}
	}

	return OCS_NEED_MORE_DATA;

}



onion_connection_status 
server_connection_chat(data, req, res) 
  void *data;
  onion_request *req;
  onion_response *res;
{
	if(usercount >= 0  && USERS[usercount]){

		if(!FOUNDUSER){
			onion_response_write0(res, "not logged in");
			return OCS_PROCESSED;
		}

		USERS[usercount]->ws = onion_websocket_new(req, res); 

		if(!USERS[usercount]->ws){
			char *view = strdup(server_get_view("socket.html"));

			if(view){
				onion_response_write0(res, view);
			}

			else{
				onion_response_write0(res, "file not found");
			}

			return OCS_PROCESSED;
		}
	}

	else{
		onion_response_write0(res, "not logged in");
		return OCS_PROCESSED;
	}

	USERS[usercount]->active = true;
	FOUNDUSER = false;
	onion_websocket_set_callback(USERS[usercount]->ws, server_websocket_chat);
	return OCS_WEBSOCKET;
}

onion_connection_status 
server_connection_home(data, req, res) 
  void *data;
  onion_request *req;
  onion_response *res;
{

	char *view = strdup(server_get_view("index.html"));

	if(view){
		onion_response_write0(res, view);
	}
	else{

		onion_response_write0(res, "file not found");
	}
	return OCS_PROCESSED;
}

onion_connection_status 
server_connection_login(_, req, res) 
  void *_;
  onion_request *req;
  onion_response *res;
{

	if (onion_request_get_flags(req) & OR_HEAD) {
		onion_response_write_headers(res);
		return OCS_PROCESSED;
	}

	const char *user_name = onion_request_get_post(req, "name");
	const char *user_password = onion_request_get_post(req, "password");

	Chatroom cr;

	if(findUser(user_name, user_password, &cr)){
		if( ( FOUNDUSER = server_create_user(strdup(user_name), cr) ) ){
			ONION_INFO("user found");
			onion_response_write0(res, 
					"<script>window.location.replace('/chat');</script>");
		}

	}
	else{
		onion_response_write0(res, "WHY ARE YOU HEARE !!!!");
	}

	return OCS_PROCESSED;

}

onion_connection_status 
server_connection_signup(_, req, res) 
  void *_;
  onion_request *req;
  onion_response *res;
{

	if (onion_request_get_flags(req) & OR_HEAD) {
		onion_response_write_headers(res);
		return OCS_PROCESSED;
	}

	const char *user_name = onion_request_get_post(req, "name");
	const char *user_password = onion_request_get_post(req, "password");
	printf("%s %s\n", user_name, user_password);

	if(createUser(user_name, user_password)){
		onion_response_write0(res, 
				"<script>window.location.replace('/login');</script>");

	}else{
		onion_response_write0(res, "user exists");

	}


	return OCS_PROCESSED;

}

void 
server_free()
{
	for(int i = 0; i < VIEWSN; i++){
		fclose(VIEWS[i].fp);
	}

	for(int i = 0; i < PUBLICN; i++){
		fclose(PUBLIC[i].fp);
	}

	free(USERS);
	dbfree();
}
