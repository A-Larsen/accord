# Chatrooms.db

```sql
CREATE TABLE sqlite_sequence(name,seq);
CREATE TABLE one (messageid INTEGER PRIMARY KEY AUTOINCREMENT, user varchar(255), message LONGBLOB, date BIGINT);
CREATE TABLE two (messageid INTEGER PRIMARY KEY AUTOINCREMENT, user varchar(255), message LONGBLOB, date BIGINT);
CREATE TABLE one_users(user varchar(255));
CREATE TABLE two_users(user varchar(255));
```

# users.db

```sql
CREATE TABLE users (name varchar(255), password varchar(255), chatrooms varchar(255));
```

like key value pairs
```
{id};{alias};{id};{alias}
```

# notes
user should have a new member called aliases for chatroom id's
SET data type for alias?

```
			CREATE TABLE %s_users(user varchar(255)); \
			INSERT INTO %s_users values(%s);", 
```
