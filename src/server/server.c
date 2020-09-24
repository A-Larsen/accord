#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "server.h"
// TODO: 
//- remove saving file descripters
//
//- need unique names for each JSON element
//
//- create a JSON file in public to get JSON objects
//	this way the server doesn't need to send as much data
//
//- need to create a better way of recieving data from the 
//	client. Maybe somoething like an XML file ?
//	libxml2 ?
// 
// it seems that onion uses libxml
// https://github.com/davidmoreno/onion/search?q=libxml&unscoped_q=libxml
//
//	https://stackoverflow.com/questions/381300/how-can-i-read-an-xml-file-into-a-buffer-in-c
//
bool loggedin = false;

const char *COOKIES_CHAT = NULL;

static bool CLOSING        = false;
static User *closinguser   = NULL;
static VIEWPASS = false;

static bool FOUNDUSER      = false;
static User **USERS;

// zero indexed count
static int usercount       = -1;

FileDataList VIEWS;
FileDataList PUBLIC;
FileDataList XML;
FileDataList POPUPS;

static bool RELOGIN = false;
static User *RELOGIN_USER = NULL;

enum diradd {NONE, STATIC};

char *
server_get_view(name)
  char *name;
{

	for(int i = 0; i < VIEWS.size; i++){
		if(!strcmp(VIEWS.list[i].name, name)){
			return VIEWS.list[i].data;
		}
	}

	return NULL;
}

void 
add_dir_callback(data, dir)
	FileData data;
	enum diradd dir;
{
	if(dir == NONE){

	}

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
		return;
	}
	
	int n = 0;

	FILE *fp;
	char tmp[MAX_FILESIZE];

	while((de = readdir(dr))){

		if(strcmp(de->d_name, ".") && strcmp(de->d_name, "..")){
			
			size_t rootdir_len = strlen(dir);
			size_t size = rootdir_len + strlen(de->d_name) + 1;

			char new[size];
			strlcpy(new, dir, rootdir_len+1);
			strlcat(new, de->d_name, size);

			fp = fopen(new, "r");
			ONION_INFO("fileno: %d", fileno(fp));
			long int len;
			len = fileToStr(fp, tmp);

			FileData data;
			data.name = strdup(de->d_name);
			data.data = strdup(tmp);
			data.length = len;

			if(!strcmp(dir, "views/")){
				VIEWS.list[n] = data;
				VIEWS.size++;
			}
			else if(!strcmp(dir, "public/")){
				PUBLIC.list[n] = data;
				server_add_static_file(urls, "public/", PUBLIC.list[n].name);
				PUBLIC.size++;
			}
			else if(!strcmp(dir, "xmlp/")){
				XML.list[n] = data;
				server_add_static_file(urls, "xmlp/", XML.list[n].name);
				XML.size++;
			}
			else if(!strcmp(dir, "popups/")){
				POPUPS.list[n] = data;
				server_add_static_file(urls, "popups/", POPUPS.list[n].name);
				POPUPS.size++;
			}

			n++;
		}
	}

	closedir(dr);
}

int 
user_create(name, password, cr)
  const char *name;
  const char *password;
  Chatrooms cr;
{
	ONION_INFO("creating users '%s'", name);

	for(int i = 0; i <= usercount; i++){
		if(USERS[i] && !strcmp(USERS[i]->name, name)){
			USERS[i] = NULL;
		}
	}

	sortn((void **)USERS, usercount+1);

	usercount++;

	ONION_INFO("*RESIZING USERS ARRAY*");
	USERS = (User **)dynamicArrayResize((void **)USERS, usercount + 1, sizeof(User));

	USERS[usercount]->id = -1;
	USERS[usercount]->ws = NULL;

	size_t len = strlen(name);

	strlcpy(USERS[usercount]->name, name, len+1);
	USERS[usercount]->name[len] = 0;
	USERS[usercount]->password = strdup(password);

	cr.chatlen = 0;
	USERS[usercount]->chatroom = cr;
	USERS[usercount]->chatroom.current = NULL;

	USERS[usercount]->active = false; 
	return 1;
}

FileData VIEWSLISTP[5];
FileData PUBLICLISTP[3];
FileData XMLLISTP[1];
FileData POPUPSLISTP[3];

int 
server_init(urls)
  onion_url *urls;
{
	VIEWS.list = VIEWSLISTP;
	VIEWS.size = 0;

	PUBLIC.list = PUBLICLISTP;
	PUBLIC.size = 0;

	XML.list = XMLLISTP;
	XML.size = 0;

	POPUPS.list = POPUPSLISTP;
	POPUPS.size = 0;

	srand(time(0));
	int rc;
	server_add_dir(NULL, "views/");
	server_add_dir(urls, "public/");
	server_add_dir(urls, "xmlp/");
	server_add_dir(urls, "popups/");
	rc = db_init();

	return rc;
}

void 
server_add_static_file(urls, route, name)
  onion_url *urls;
  char *route;
  char *name;
{
	int routelen = strlen(route);
	int len = routelen + strlen(name) + 1;

	char file[len];
	strlcpy(file, route, routelen+1);
	strlcat(file, name, len);

	FILE *fp = fopen(file, "r");

	char filestr[MAX_FILESIZE];
	fileToStr(fp, filestr);

	onion_url_add_static(urls, file, filestr, HTTP_OK);
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
				onion_websocket_printf(USERS[i]->ws, "%s", message);
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
				onion_websocket_printf(USERS[user->cu.indices[i]]->ws, "%s", message);
		}
	}

}

void server_websocket_get_connectedUsers(user, exclude)
  User *user; 
  char *exclude;
{
	ConnectedUsers cu;

	Clist c = db_get_chatroom_users(user->chatroom.current);
	char *a[usercount+1];
	int len = 0;
	cu.indices = NULL;


	for(int i = 0; i < c.len; i++){
		for(int j = 0; j <= usercount; j++){
			if(USERS[j] && !strcmp(USERS[j]->name, c.list[i]) && strcmp(USERS[j]->name, exclude)){

				cu.indices = (int *)dynamicArrayResize2(cu.indices, len + 1, sizeof(int));

				cu.indices[len] = j;
				len++;
				a[i] = USERS[j]->name;
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
					onion_websocket_printf(USERS[j]->ws, "%s", tmp);
				}
			}
		}
	}

	free(tmp);
	free(ja);
}


int 
user_close(user)
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
}

void
server_websocket_send_chatrooms(user)
  User *user;
{
	if(user->chatroom.rooms.len == 0)
		return;

	char *tmp = NULL;

	char *a = arrayToJSONArray(user->chatroom.rooms.items, user->chatroom.rooms.len);

	if(a){
		asprintf(&tmp, "{\"initchatrooms\":%s}", a);
		printf("JSON ROOMS: %s\n", tmp);
		onion_websocket_printf(user->ws, "%s", tmp);
		free(tmp);
		free(a);
	}

}

void 
server_add_friend(user, md)
  User *user;
  ClientData md;
{
	printf("ADDING A FRINED: %s\n", md.addfriend.name);
	printf("ADDING A FRINED ID: %s\n", md.addfriend.roomid);
	if(db_check_if_user_exists(md.addfriend.name)){
		db_insert_into_chatroom_users(md.addfriend.roomid, md.addfriend.name);
		char * chatrooms = db_select_chatroom(md.addfriend.name);
		db_update_users_chatrooms(chatrooms, md.addfriend.name, md.addfriend.roomid, md.addfriend.roomname);
	}else{
		onion_websocket_printf(user->ws, "{\"error\": \"user does not exist\"}");
	}

}

void
server_load_chatroom(user, md)
  User *user;
  ClientData md;
{
	ONION_INFO("INIT CHATROOM: %s", md.initchatroom);
	user->chatroom.current = strdup(md.initchatroom);
	server_websocket_get_connectedUsers(user, "");
	db_init_login(&user->chatroom, user->ws);
}

void 
server_send_message(user, md)
  User *user;
  ClientData md;
{
	user->chatroom.current = strdup(md.message.chatroom);
	char *message = parseToJSONforClient(user->name, md);

	server_websocket_printf_connected(user, message);
	db_store_message(&user->chatroom,
				user->name, 
				md.message.content, 
				md.message.lldate);
}

onion_connection_status 
server_websocket_chat(data, ws, data_ready_len) 
  void *data;
  onion_websocket *ws;
  ssize_t data_ready_len;
{
	/* printf("what\n"); */


	if(CLOSING && closinguser){
		user_close(closinguser);
		return OCS_CLOSE_CONNECTION;
	}

	char tmp[500];
	if ((long unsigned int)data_ready_len > sizeof(tmp))
		data_ready_len = sizeof(tmp) - 1;

	int len = onion_websocket_read(ws, tmp, data_ready_len);

	if (len <= 0) {
		ONION_ERROR("Error reading data: %d: %s (%d)", errno, strerror(errno),
					data_ready_len);
		return OCS_NEED_MORE_DATA;
	}
	tmp[len] = 0;

	ClientData md;
	parseXML(tmp, &md);
	if(loggedin){
		printf("WAIT IM LOGGED IN\n");
	}
	ONION_INFO("FOUND CHATROOM %s\n", md.message.chatroom);

	User *user = getUser(md.user.id);

	if(USERS[usercount] && USERS[usercount]->id == -1){
		USERS[usercount]->id = md.user.id;

		if(USERS[usercount]->chatroom.rooms.len > 0)
			server_websocket_send_chatrooms(USERS[usercount]);

		return OCS_NEED_MORE_DATA;
	}
	
	if(user){
		ONION_INFO("FOUND USER");
		if(md.user.closing){
			/* onion_websocket_printf(user->ws, "{\"reload\": \"/chat\", \"getInfo\": [\"userid\"]}"); */
			/* ONION_INFO("ABOUT TO CLOSE"); */
			/* closinguser = user; */
			/* CLOSING = true; */
			/* return OCS_NEED_MORE_DATA; */
		}

		else if(md.addfriend.roomname){
			server_add_friend(user, md);
		}

		else if(md.addroom.id){
			printf("ROOMID: %s\n", md.addroom.id);
			printf("ROOM ALIAS: %s\n", md.addroom.alias);
			db_add_chatroom(md.addroom.id, md.addroom.alias, user->name);
			RELOGIN = true;
			RELOGIN_USER = user;

			onion_websocket_printf(user->ws, "{\"reload\": \"/chat\"}");
		}

		else if(md.initchatroom){
			server_load_chatroom(user, md);
		}

		else if(md.message.chatroom){
			ONION_INFO("sending message?");
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
	/* printf("RESET\n"); */
	if(loggedin){
		printf("GETTING THERE\n");
	}

	if(OPTIONS & AUTOLOGIN){
		printf("OPTION_VALUE: '%s'\n", OPTION_VALUE);
		char * pass = admin_search(OPTION_VALUE);

		if(!pass){
			fprintf(stderr, "could not find password for admin");
			exit(EXIT_FAILURE);
		}

		Chatrooms cr;
		db_find_user(OPTION_VALUE, pass, &cr);
		FOUNDUSER = user_create(OPTION_VALUE, pass, cr);
	}
	else if(RELOGIN && RELOGIN_USER){
		Chatrooms cr;
		db_find_user(RELOGIN_USER->name, RELOGIN_USER->password, &cr);
		FOUNDUSER = user_create(RELOGIN_USER->name, RELOGIN_USER->password, cr);
		RELOGIN = false;
	}

	if(usercount >= 0  && USERS[usercount]){

		if(!FOUNDUSER){
			// we need a notLoggedin.html
			// to handle things

			if(COOKIES_CHAT){

				ONION_INFO("COOKIES '%s'\n", COOKIES_CHAT);
				char *cookiecpy = strdup(COOKIES_CHAT);
				char *value = parseCookie(cookiecpy);

				ONION_INFO("COOKIES AFTER PARSE '%s'\n", COOKIES_CHAT);

				if(value){
					loggedin = !strcmp(value, "true");
					ONION_INFO("LOGGED IN: %d", loggedin);
				}

				free(cookiecpy);
				/* loadHTML(res, "notloggedin.html"); */
				/* onion_response_write0(res, */ 
				/* 		server_get_view("notloggedin.html")); */
				/* onion_response_write0(res, */ 
				/* 		 "{\"reload\": \"/chat\"}"); */
				/* return OCS_PROCESSED; */ 

			}else{

				onion_response_write0(res, "not logged in");
				return OCS_PROCESSED;
			}

		}


		USERS[usercount]->ws = onion_websocket_new(req, res); 

		if(!USERS[usercount]->ws){
			onion_dict *h = onion_response_get_headers(res);
			onion_response_add_cookie(res, "loggedin", "true", -1, NULL, 
															NULL, 0);
			COOKIES_CHAT = strdup(onion_dict_get(h, "Set-Cookie"));
			onion_response_write0(res, 
					server_get_view("socket.html"));
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

	free(view);

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

	if(!user_password || !user_password){
		onion_response_write0(res, "invalid form");
		return OCS_PROCESSED;
	}

	Chatrooms cr;

	if(db_find_user(user_name, user_password, &cr)){
		if( ( FOUNDUSER = user_create(user_name, user_password, cr) ) ){
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

	if(!user_name || !user_password){
		onion_response_write0(res, "invalid form");
		return OCS_PROCESSED;
	}

	printf("%s %s\n", user_name, user_password);

	if(db_create_user(user_name, user_password)){
		onion_response_write0(res, 
				"<script>window.location.replace('/login');</script>");

	}else{
		onion_response_write0(res, "user exists");

	}

	return OCS_PROCESSED;
}

onion_connection_status 
server_connection_sitemap(_, req, res) 
  void *_;
  onion_request *req;
  onion_response *res;
{

	if (onion_request_get_flags(req) & OR_HEAD) {
		onion_response_write_headers(res);
		return OCS_PROCESSED;
	}

	onion_response_set_header(res, "Content-Type", "text/xml");

	onion_response_write0(res, 
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<story><test>heyy</test><one><thing type=\"string\" len=\"4\">nice</thing></one></story>");

	return OCS_PROCESSED;
}

void 
server_free()
{
	free(USERS);
	db_free();
}
