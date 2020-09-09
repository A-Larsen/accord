#include <onion/onion.h>
#include "database.h"
#include "../parse.h"
#include "../sort.h"
/* #include "../parse.h" */
typedef struct _ConnectedUsers{
	int *indices;
	int len;
} ConnectedUsers;

typedef struct _User{
	int id;
	char name[10];
	onion_websocket *ws;
	bool active;
	Chatroom chatroom;
	ConnectedUsers cu;
} User;

int usercount       = -1;
bool CLOSING        = false;
User *closinguser   = NULL;

bool FOUNDUSER      = false;
User **USERS;

int 
closeUser(user)
  User *user;
{

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
	}
	else{
		onion_websocket_printf(user->ws, "{\"error\": \"user does not exist\"}");
	}

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

