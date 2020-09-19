cfiles = mem.c parse.c sort.c convert.c server/database.c server/server.c main.c
CFLAGS = -lm -lsqlite3 -lpthread -lonion -lxml2 -Wall -Wextra 
buildpath = build

build: main.c server/server.c parse.c sort.c
	${CC} ${CFLAGS} -o a $(buildpath)/a.out $(cfiles) 

run: main.c server/server.c parse.c sort.c
	${CC} ${CFLAGS} -o $(buildpath)/a.out $(cfiles) 
	$(buildpath)/a.out

debug: main.c server/server.c parse.c server/database.c
	${CC} -g ${CFLAGS} -o $(buildpath)/a.out $(cfiles) 
	gdb $(buildpath)/a.out 

clean:


.PHONY: clean
