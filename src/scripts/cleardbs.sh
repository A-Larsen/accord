#!/bin/sh

# make sure to not allow special character as username in server
sqlite3 databases/users.db 'UPDATE users SET chatrooms = ""'
sqlite3 databases/users.db 'DELETE FROM users WHERE name != "#ONE" AND name != "#TWO"'

if [ -f "databases/chatrooms.db" ]; then
	doas rm -r databases/chatrooms.db 
fi
