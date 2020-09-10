
cfiles = mem.c parse.c sort.c convert.c server/database.c server/server.c main.c
CFLAGS = -lm -lsqlite3 -lpthread -lonion -Wall -Wextra 

build: main.c server/server.c parse.c sort.c
	${CC} ${CFLAGS} -o a.out $(cfiles) 

run: main.c server/server.c parse.c sort.c
	${CC} ${CFLAGS} -o a.out $(cfiles) 
	xdotool keydown Super_L 2 keyup Super_L 2 keydown Super_L j keyup Super_L j 
	chromium "0.0.0.0:8080/signup"
	./a.out

debug: main.c server/server.c parse.c server/database.c
	${CC} -g ${CFLAGS} -o a.out $(cfiles) 
	xdotool keydown Super_L 2 keyup Super_L 2 keydown Super_L j keyup Super_L j 
	chromium "0.0.0.0:8080/signup"
	gdb a.out 

clean:
	./cleardbs.sh

.PHONY: clean
