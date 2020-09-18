#!/bin/sh

sqlite3 ../server/users.db "DELETE FROM users;" 

if [ -f "../server/chatrooms.db" ]; then
	doas rm -r ../server/chatrooms.db 
fi
