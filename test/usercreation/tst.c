#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <onion/onion.h>
#include <time.h>

typedef struct _User{
	int id;
	const char *name;
	const char *color;
	onion_websocket *ws;
	bool active;
} User;

static int usercount = -1;
static User **USERS;

User *getUser(int id){
	for(int i = 0; i <= usercount; i++){
		if(USERS[i] && USERS[i]->id == id){
			return USERS[i];
		}
	}
	return NULL;
}

void createUser(char *name){
	usercount++;

	USERS = 
		usercount == 0 ? 
		(User **)malloc(sizeof(User)) : 
		(User **)realloc(USERS, sizeof(User) * (usercount + 1));

	USERS[usercount] = (User *)malloc(sizeof(User));
	USERS[usercount]->color = "red";
	USERS[usercount]->id = -1;
	USERS[usercount]->ws = NULL;
	USERS[usercount]->name = name; 
	USERS[usercount]->active = false; 
}


void clientCreatews(){
	static int seed = 1;
	srand(seed++);
	USERS[usercount]->id = rand();
	USERS[usercount]->active = true;
}

void close(int id){
	User *user = getUser(id);

	printf("closing: %s %d\n", user->name, user->id);

	for(int i = 0; i <= usercount; i++){
		if(USERS[i] == user){
			printf("setting user with id %d to NULL", USERS[i]->id);
			USERS[i] = NULL;
		}
	}

	if(usercount < 0){
		free(USERS);
	}
}

int main(){
	createUser("joe");
	clientCreatews();

	createUser("bill");
	clientCreatews();

	createUser("john");
	clientCreatews();

	for(int i = 0; i <= usercount; i++){
		printf("%d: %s\n", i, USERS[i]->name);
		printf("%d: %d\n", i, USERS[i]->id);
	}

	printf("\nbefore close: %d\n", USERS[1]->id);

	close(USERS[1]->id);

	createUser("jake");
	clientCreatews();

	if(USERS[1])
		printf("after close: %d\n", USERS[1]->id);

	putchar('\n');

	for(int i = 0; i <= usercount; i++){
		if(USERS[i]){
			printf("%d: %s\n", i, USERS[i]->name);
			printf("%d: %d\n", i, USERS[i]->id);
		}
	}
}
