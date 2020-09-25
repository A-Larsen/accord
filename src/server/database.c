#include "database.h"

static sqlite3 *usersdb;
static sqlite3 *chatroomsdb;

int db_step_callback(sqlite3_stmt *stmt, void *data)
{return sqlite3_step(stmt); };

void
sqlite_prep_stmt(db, sql, callback, data)
  sqlite3 *db;
  char *sql;
  int (*callback) (sqlite3_stmt *, void *);
	void *data;
{
	sqlite3_stmt *stmt;

	if(sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK){
		ONION_ERROR("Failed sql: %s\n %s\n", 
					sqlite3_errmsg(db),
					sql);
		return;
	}

	int ret = callback(stmt, data);
	if((ret != SQLITE_DONE) && (ret != SQLITE_ROW)){ 
		ONION_ERROR("prepare failed: %s\n", 
				sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);
}

static int callback_search_user(data, argc, argv, azColName)
  void *data;
  int argc;
  char **argv;
  char **azColName;
{
	int i;
	char **userdata = (char **)data;

	bool foundname = false, foundpassword = false;

	for(i = 0; i < argc; i++){

		if(argv[i]){
			if(!strcmp(azColName[i], "name")){
				if(!strcmp(argv[i], userdata[0])){
					foundname = true;
				}
			}

			if(!strcmp(azColName[i], "password")){
				if(!strcmp(argv[i], userdata[1])){
					foundpassword = true;
				}
			}

			if(foundname && 
			  foundpassword && 
			  !strcmp(azColName[i], "chatrooms") && 
			  argv[i])
			{
				userdata[2] = malloc(1);
				*userdata[2] = 'a';
				int len = strlen(argv[i])+1;
				userdata[3] = malloc(sizeof(char) * len);
				userdata[3] = strdup(argv[i]);
				userdata[3][len] = 0;
				break;
			}
		}
	}

	return 0;
}


int db_init()
{
	static int rc;

	rc = sqlite3_open("databases/users.db", &usersdb);

	if(rc){
		ONION_ERROR("Can't open users database: %s\n", 
				sqlite3_errmsg(usersdb));
		return 0;
	}

	rc = sqlite3_open("databases/chatrooms.db", &chatroomsdb);

	if(rc){
		ONION_ERROR("Can't open chatrooms database: %s\n", 
				sqlite3_errmsg(chatroomsdb));
		return 0;
	}

	return 1;
}

int 
db_find_user(name, password, cr)
  const char *name;
  const char *password;
  Chatrooms *cr;
{

	if(!name || !password ){
		return 0;
	}

	char *sql = "SELECT * FROM users";
	char *zErrMsg = 0;
	int rc;

	char *userdata[4];
	userdata[0] = strdup(name);
	userdata[1] = strdup(password);
	userdata[2] = "b";
	userdata[3] = NULL;

	rc = sqlite3_exec(usersdb, 
					 sql, 
					 callback_search_user, 
					 (void *)userdata, 
					 &zErrMsg);

	if(rc != SQLITE_OK){
		ONION_ERROR("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}

	rc = *userdata[2] == 'a';

	if(rc){
		parseArrayList(userdata[3], &cr->rooms);
	}

	return rc;
}

int 
db_store_message(cr, name, msg, date)
  Chatrooms *cr;
  const char *name;
  char *msg;
  long int date;
{

	char parse[512];

	int i = 0;

	for(; *msg != 0; msg++){
		parse[i] = *msg;

		if(*msg == '\''){
			parse[++i] = '\'';
		}

		i++;

	}
	parse[i] = 0;

	char *sql = NULL;

	ONION_INFO("MESSAGE: %s\n", parse);

	asprintf(&sql, 
			"INSERT INTO %s (user, message, date) VALUES ('%s', '%s', %ld)", 
			cr->current, 
			name, 
			parse, 
			date);

	sqlite_prep_stmt(chatroomsdb, sql, db_step_callback, NULL);

	free(sql);

	return 1;
}


static int 
callback_search_chatrooms(data, argc, argv, azColName)
  void *data;
  int argc;
  char **argv;
  char **azColName;
{
	int i;
	bool founduser = false, foundmessage = false, founddate = false;
	Chatrooms *cr = ((Chatrooms **)data)[0];
	onion_websocket *ws = ((onion_websocket **)data)[1];
	char *user = NULL;
	ClientData md;

	for(i = 0; i < argc; i++){

		if(argv[i]){
			if(!strcmp(azColName[i], "user")){
				int len = strlen(argv[i])+1;
				user = malloc(sizeof(char) *len);
				user = strdup(argv[i]);
				user[len] = 0;
				founduser = true;
			}

			if(!strcmp(azColName[i], "message")){
				md.message.content = strdup(argv[i]);
				foundmessage = true;
			}

			if(!strcmp(azColName[i], "date")){
				md.message.sdate = ts_to_readable(atoll(argv[i]));
				md.message.lldate = 0;
				founddate = true;
			}

		}
		if(founduser && foundmessage && founddate){

			md.message.chatroom = strdup(cr->current);
			char *message = parseToJSONforClient(user, md);

			onion_websocket_printf(ws, message);

			if(user)
				free(user);

			if(message)
				free(message);

			break;
		}
	}

	return 0;
}

int 
db_init_login(cr, ws)
  Chatrooms *cr;
  onion_websocket *ws;
{

	char *zErrMsg = 0;
	int rc = 0;

	
	sqlite3_stmt *res;

	char *sql1 = NULL;
	asprintf(&sql1, "SELECT seq FROM sqlite_sequence WHERE name='%s'", cr->current);

	rc = sqlite3_prepare_v2(chatroomsdb, sql1, -1, &res, 0);
	if(rc != SQLITE_OK){
		ONION_ERROR("Failed to fetch data: %s\n", sqlite3_errmsg(chatroomsdb));
	}

	rc = sqlite3_step(res);

	if(rc == SQLITE_ROW){
		cr->chatlen = sqlite3_column_int64(res, 0); 
	}

	ONION_INFO("last messageid: %lld\n", cr->chatlen);

	sqlite3_finalize(res);

	char *sql2 = NULL;

	asprintf(&sql2, "SELECT * FROM %s WHERE messageid > %lld", cr->current, cr->chatlen - 20);

	void *data[2];
	data[0] = cr;
	data[1] = ws;

	rc = sqlite3_exec(chatroomsdb, 
					 sql2, 
					 callback_search_chatrooms, 
					 (void *)data, &zErrMsg);

	if(rc != SQLITE_OK){
		ONION_ERROR("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}

	free(sql1);
	free(sql2);

	return 1;
}

int
db_get_chatroom_users_callback(stmt, data)
  sqlite3_stmt *stmt;
  void *data;
{
	int rc = 0;
	Clist *c = ((Clist **)data)[0];
	char *current = ((char **)data)[1];

	while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
		ONION_INFO("%s_users row: %s\n", current, sqlite3_column_text(stmt, 0));
		c->list = (char **)dynamicArrayResize((void **)c->list, c->len + 1, sizeof(char *));

		const unsigned char * text = sqlite3_column_text(stmt, 0);
		c->list[c->len] = strdup((char *)text);
		c->list[c->len][strlen((char *)text)] = 0;
		c->len++;
	}
	return rc;


} Clist
db_get_chatroom_users(current)
  char *current;
{
	Clist c;
	c.len = 0;
	c.list = NULL;

	void *data[2];
	data[0] = &c;
	data[1] = current;

	char *sql = NULL;
	asprintf(&sql, "SELECT * FROM %s_users", current);

	sqlite_prep_stmt(chatroomsdb, sql, db_get_chatroom_users_callback, data);
	free(sql);
	return c;
}

static int
db_create_table_chatroom(id)
	char *id;
{
	char *sql = NULL;

	asprintf(&sql, 
			"CREATE TABLE %s ("
					"messageid INTEGER PRIMARY KEY AUTOINCREMENT,"
					"user varchar(255), "
					"message LONGBLOB, "
					"date BIGINT);",
			id);

	sqlite_prep_stmt(chatroomsdb, sql, 
			db_step_callback, NULL);

	free(sql);
	return SQLITE_DONE;
}

static int
db_create_table_chatroom_users(id)
  char *id;
{
	char *sql = NULL;

	asprintf(&sql, 
			"CREATE TABLE %s_users(user varchar(255))",
			id);

	sqlite_prep_stmt(chatroomsdb, sql, 
			db_step_callback, NULL);

	free(sql);

	return SQLITE_DONE;
}

int
db_check_if_user_in_chatroom_callback(stmt, data)
	sqlite3_stmt *stmt;
	void *data;
{
	int rc = 0;
	char *current = ((char **)data)[0];
	char *check  = ((char **)data)[1];
	bool *found  = ((bool **)data)[2];

	while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
		ONION_INFO("%s_users row: %s\n", current, sqlite3_column_text(stmt, 0));
		if(!strcmp((char *)sqlite3_column_text(stmt, 0), check)){
			*found = true;
			break;
		}
	}

	return rc;

} static bool 
db_check_if_user_in_chatroom(current, check)
  char *current;
  char *check;
{

	bool found = false;; 

	void *data[3];
	data[0] = current;
	data[1] = check;
	data[2] = &found;

	char *sql = NULL;
	asprintf(&sql, "SELECT * FROM %s_users", current);

	sqlite_prep_stmt(chatroomsdb, sql, 
			db_check_if_user_in_chatroom_callback, data);

	free(sql);

	return found;
}

int
db_check_if_user_exists_callback(stmt, data)
  sqlite3_stmt *stmt;
  void *data;
{
	int rc = 0;
	char *name = ((char **)data)[0];
	ONION_INFO("NAME: %s\n", name);
	bool *found = ((bool **)data)[1];


	while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
		if(!strcmp((char *)sqlite3_column_text(stmt, 0), name)){
			*found = true;
			break;
		}
	}

	return rc; 

} bool
db_check_if_user_exists(name)
  char *name;
{

	bool found = false;
	void *data[2];
	data[0] = name;
	data[1] = &found;

	sqlite_prep_stmt(usersdb, "SELECT name FROM users", 
			db_check_if_user_exists_callback, data);

	return found;
}

// do not insert if the name already exists
int
db_insert_into_chatroom_users(id, name)
  char *id;
  char *name;
{
	if(!db_check_if_user_exists(name) || db_check_if_user_in_chatroom(id, name))
		return 0;


	char *sql = NULL;

	asprintf(&sql, 
			"INSERT INTO %s_users values('%s')", 
			id, name);

	sqlite_prep_stmt(chatroomsdb, sql, 
			db_step_callback, NULL);

	free(sql);

	return SQLITE_DONE;
}


int
db_select_chatroom_callback(stmt, data)
	sqlite3_stmt *stmt;
	void *data;
{
	int rc = sqlite3_step(stmt);
	char **chatrooms = (char **)data;

	if(rc == SQLITE_ROW){
		*chatrooms = strdup((char *)sqlite3_column_text(stmt, 0));
	}

	return SQLITE_DONE; 

} char * 
db_select_chatroom(name)
  char *name;
{
	char *sql = NULL;
	asprintf(&sql, "SELECT chatrooms FROM users WHERE name='%s'", name);
	char *chatrooms = NULL;

	sqlite_prep_stmt(usersdb, sql, 
			db_select_chatroom_callback, (void *)&chatrooms);

	return chatrooms;
}

int 
db_update_users_chatrooms(chatrooms, name, id, alias)
  char *chatrooms;
  char *name;
  char *id;
  char *alias;
{
	if(!chatrooms)
		return 0;

	char *cat = NULL;
	if(!strcmp(chatrooms, "")){
		asprintf(&cat, "%s;%s;", alias, id);
	}else{
		asprintf(&cat, "%s%s;%s;", chatrooms, alias, id);
	}

	char *sql = NULL;

	ONION_INFO("sql cat: %s\n", cat);
	ONION_INFO("sql alias: %s\n", alias);
	asprintf(&sql, "UPDATE users SET chatrooms='%s' WHERE name='%s'", cat, name);
	ONION_INFO("sql: %s\n", sql);
	sqlite_prep_stmt(usersdb, sql, 
			db_step_callback, NULL);

	free(sql);
	free(cat);
	return SQLITE_DONE;
}

int
db_add_chatroom(id, alias, name)
  char *id;
  char *alias;
  char *name;
{	

	db_create_table_chatroom(id);
	db_create_table_chatroom_users(id);
	db_insert_into_chatroom_users(id, name);

	char * chatrooms = db_select_chatroom(name);

	db_update_users_chatrooms(chatrooms, name, id, alias);

	free(chatrooms);

	return SQLITE_DONE;
}

int 
db_create_user(name, password)
  const char *name;
  const char *password;
{
	if(db_check_if_user_exists((char *)name))
		return 0;

	char *sql = NULL;

	asprintf(&sql, 
			"INSERT INTO users (name, password, chatrooms) VALUES ('%s', '%s', '')", 
			name, password);

	sqlite_prep_stmt(usersdb, sql, 
			db_step_callback, NULL);

	free(sql);

	return 1;
}

void
db_free()
{
	sqlite3_close(usersdb);
	sqlite3_close(chatroomsdb);
}
