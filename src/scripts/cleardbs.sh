#!/bin/sh

# make sure to not allow special character as username in server
sqlite3 databases/users.db "DELETE FROM users WHERE name != #ADMIN_ONE;" 

if [ -f "databases/chatrooms.db" ]; then
	doas rm -r databases/chatrooms.db 
fi
