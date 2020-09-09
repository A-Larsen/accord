
cfiles = mem.c parse.c sort.c convert.c server/database.c server/server.c main.c
CFLAGS = -lm -lsqlite3 -lpthread -lonion -Wall -Wextra 

build: main.c server/server.c parse.c sort.c
	${CC} ${CFLAGS} -o a.out $(cfiles) 

run: main.c server/server.c parse.c sort.c
	${CC} ${CFLAGS} -o a.out $(cfiles) && ./run.sh

debug: main.c server/server.c parse.c server/database.c
	${CC} -g ${CFLAGS} -o a.out $(cfiles) && ./debug.sh

clean: a.out
	sqlite3 server/users.db "DELETE FROM users;" 
	doas rm -r server/chatrooms.db 
	doas rm a.out

.PHONY: clean
