cfiles = mem.c parse.c sort.c convert.c server/database.c server/server.c main.c
CFLAGS = -lm -lsqlite3 -lpthread -lonion -lxml2 -lbsd -Wall -Wextra 
buildpath = build

build: main.c server/server.c parse.c sort.c
	export ONION_LOG=
	${CC} ${CFLAGS} -o $(buildpath)/a.out $(cfiles) 

run: main.c server/server.c parse.c sort.c
	export ONION_LOG=
	${CC} ${CFLAGS} -o $(buildpath)/a.out $(cfiles) 
	$(buildpath)/a.out

debug: main.c server/server.c parse.c server/database.c
	export ONION_LOG=
	${CC} -g ${CFLAGS} -o $(buildpath)/a.out $(cfiles) 
	gdb $(buildpath)/a.out 

clean:


# gcc -c ../mem.c ../parse.c ../sort.c ../convert.c ../server/database.c ../server/server.c ../main.c
# gcc -lm -lsqlite3 -lpthread -lonion -lxml2 -Wall -Wextra ../mem.c ../parse.c ../sort.c ../convert.c ../server/database.c ../server/server.c ../main.c
# gcc -lm -lsqlite3 -lpthread -lonion -lxml2 -Wall -Wextra mem.o parse.o sort.o convert.o database.o server.o main.o 

.PHONY: clean
