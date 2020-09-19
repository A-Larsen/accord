#!/bin/sh

sqlite3 databases/users.db "DELETE FROM users;" 

if [ -f "databases/chatrooms.db" ]; then
	doas rm -r databases/chatrooms.db 
fi
