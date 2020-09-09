#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *chatroomlist = "yes;what;hello;";

char **parseChatRoomList(char *list){

	char *arrayp = list;

	char **chatrooms = NULL;
	int cidx = 0;

	int arraysize = 0;

	for(;*list != 0; list++){

		if(*list == ';'){
			int tmpsize = list - arrayp;
			int move = tmpsize;

			arraysize += move;

			if(!chatrooms)
				chatrooms = malloc(sizeof(char) * move);
			else
				chatrooms = realloc(chatrooms, sizeof(char) * arraysize);

			chatrooms[cidx] = malloc(sizeof(char) * move);
			strncpy(chatrooms[cidx], arrayp, move);
			chatrooms[cidx][move] = 0;
			cidx++;

			arrayp += tmpsize+1;
		}
	}

	if(chatrooms){
		printf("chatrooms: %s\n", chatrooms[0]);
	}

	return chatrooms;
}


int main(){
	char ** parse = parseChatRoomList(chatroomlist);
	printf("%s\n", parse[2]);

}


/* char **chatrooms; */
/* int main(){ */

/* 	int size = 4; */
/* 	chatrooms = malloc(sizeof(char)*size); */
/* 	chatrooms[0] = malloc(sizeof(char)*size); */
/* 	chatrooms[0] = "hello"; */


/* 	chatrooms = realloc(chatrooms, sizeof(char)*size+2); */
/* 	chatrooms[1] = malloc(sizeof(char)*2); */
/* 	chatrooms[1] = "no"; */

/*     printf("%s\n", chatrooms[0]); */
/*     printf("%s\n", chatrooms[1]); */
/* } */
