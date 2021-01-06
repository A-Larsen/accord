cfiles = mem.c parse.c sort.c convert.c server/database.c server/server.c main.c
CFLAGS = -lm -lsqlite3 -lpthread -lonion -lxml2 -lbsd -Wall -Wextra 
buildpath = build

build: $(cfiles)
	export ONION_LOG=
	${CC} ${CFLAGS} -o $(buildpath)/a.out $(cfiles) 

cli: $(cfiles) cli/cli.c
	export ONION_LOG='noinfo'
	${CC} -DCLI ${CFLAGS} -o $(buildpath)/a.out $(cfiles) cli/cli.c

run: $(cfiles)
	export ONION_LOG=
	${CC} ${CFLAGS} -o $(buildpath)/a.out $(cfiles) 
	$(buildpath)/a.out

debug: $(cfiles)
	export ONION_LOG=
	${CC} -g ${CFLAGS} -o $(buildpath)/a.out $(cfiles) 
	gdb $(buildpath)/a.out 

clean:


# gcc -c ../mem.c ../parse.c ../sort.c ../convert.c ../server/database.c ../server/server.c ../main.c
# gcc -lm -lsqlite3 -lpthread -lonion -lxml2 -Wall -Wextra ../mem.c ../parse.c ../sort.c ../convert.c ../server/database.c ../server/server.c ../main.c
# gcc -lm -lsqlite3 -lpthread -lonion -lxml2 -Wall -Wextra mem.o parse.o sort.o convert.o database.o server.o main.o 

.PHONY: clean
