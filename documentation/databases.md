# Databases

## chatrooms.db


### Chatroom table
There is one table for each chatroom. 

The chatroom table name is always expressed as just an id which is a 20 
lowercase letter characters that are randomly generated.


```sql
ecyacndotuvundxaqmpk (messageid INTEGER PRIMARY KEY AUTOINCREMENT, 
							user varchar(255), message LONGBLOB, date BIGINT);
```
|           |                                                                            |
|:---------:|----------------------------------------------------------------------------|
| messageid | Id of message.  This number wil increment by one  with each message added. |
|   user    | This is the name of the user that sent the message.                        |
|  message  | This is the message the user sent in HTML fromat.                          |
|   date    | The date the message was sent represented in a unix timestamp.             |

### Chatroom users table

There is also a seperate table for each chatroom that has all of the users that
belong to that room.

The chatroom users table is expressed with the id for the chatroom followed by 
"_users"

*id*_users

```sql
ecyacndotuvundxaqmpk_users(user varchar(255));
```
|       |                                           |
| :---: | ----------------------------------------- |
| user  | name of user that belongs to the chatroom |


## users.db

### Users table
This holds a collection of all the users in the server in the users table.

```sql
CREATE TABLE users (name varchar(255), password varchar(255), chatrooms varchar(255));
```
- encryption not yet implemented

|           |                                                                                              |
|:---------:|----------------------------------------------------------------------------------------------|
|   name    | The name of the user                                                                         |
| password  | encryped password of the user                                                                |
| chatrooms | A list of chatrooms delimeted with a ';'. There must be a trailing ';'  for the last element |


