# FixMe

if I open some pages and then close those pages in the reverse order the were opend then they close
correctly but do not otherwise.

## solution attempt 1
- close the socket
- sort all NULL element to the end of array
- and then shrink `USERS` array thus shrinking `usercount`

```c
void asort(char **data, int len){
	for(int i = 0; i < len; i++){
		for(int y = 0; y < len; y++){
			if(!data[y]){
				data[y] = data[i];
				data[i] = NULL;
			}
		}
	}
}
```
  
  
The first user that logins in has their credentails appropriately evaluated. Any
user after the first no longer needs a password


```c
bool found = false;
```

it seems that this global varaible `found` in database .c is causing the issue

```c
static int callback(void *data, int argc, char **argv, char **azColName){
	int i;
	char **userdata = (char **)data;

	bool foundname = false;
	bool foundpassword = false;
	found = false; // <<--
```

...nope

I'm going to try to set it to `false` at the begining of the callback for
`findUser`

```c
	char *userdata[2];
	userdata[0] = strdup(name);
	userdata[1] = strdup(password);
	userdata[2] = 0;
	...
	rc = sqlite3_exec(db, sql, callback, (void *)userdata, &zErrMsg);
	...
	if(*userdata[2]){
		return 1;
	}
```

tried to set `userindex[2]` to a value that can be interpreted as a truth value.
it seems to cause a seg fault when login is incorrect


## too many users
seg fault happens when you create the max number of users and get the "chat room
full or not logged in" then continue to a message as another user on another
page. I belive this is becuase it exited the websocket callback. once it exits
the websocket callback it has to make a new `Users` object. once it creates the
user object it makes a new websocket on the server side for the user but not on
the client side.

maye make sure to not make a new websocket on ther server side if the client is
already logged in (`active`).

<pre>ORRRRR</pre>

make a new websocket on the clientside

yep that fixed it


look at all the onion options
```
$ gcc -l onion
onion            onioncpp         onioncpp_static  onion_extras     onion_static  

```

## reloading page when logged in causes seg fault
create cookies to determine if user is logged in


TODO:

- make a differnt `Data` object for views for each user
  so you can change the page at anytime

- parse messages to esape some character

- fix timestamps

# Future me

try to learn sql3 more see if there is a better way t retrieve information. for
example. 

- do i have to only retrieve strings 
	what about integers ?

- what is the most appropriate datatype for messages ?


# recent error
```
SEND CHATROOMS: well
SEND CHATROOMS: jtckpbubbuzcbjfjqpjy
all chatrooms sent
JSON ROOMS: {"initchatrooms":["well","jtckpbubbuzcbjfjqpjy"]}
addroom: not
*ADDING A ROOM !*
ROOMID: pmsuexcsysnilqsbrncs
ROOM ALIAS: not
SQL: CREATE TABLE pmsuexcsysnilqsbrncs (messageid INTEGER PRIMARY KEY AUTOINCREMENT,user varchar(255), message LONGBLOB, date BIGINT);
SQL: CREATE TABLE pmsuexcsysnilqsbrncs_users(user varchar(255))
SQL: INSERT INTO pmsuexcsysnilqsbrncs_users values('kay')
sql: UPDATE users SET chatrooms='well;jtckpbubbuzcbjfjqpjy;not;pmsuexcsysnilqsbrncs;' WHERE name='kay'
addroom: quite
*ADDING A ROOM !*
ROOMID: ibujlatbpcafosatpclo
ROOM ALIAS: quite
munmap_chunk(): invalid pointer
Aborted (core dumped)
make: *** [Makefile:5: run] Error 134

```

```
sqlite> SELECT * FROM users;
joe|2322|john;zygzpiifxwbrrqrcvssr;
john|1111|john;zygzpiifxwbrrqrcvssr;
kay|3333|


CREATE TABLE sqlite_sequence(name,seq);
CREATE TABLE one (messageid INTEGER PRIMARY KEY AUTOINCREMENT, user varchar(255), message LONGBLOB, date BIGINT);
CREATE TABLE two (messageid INTEGER PRIMARY KEY AUTOINCREMENT, user varchar(255), message LONGBLOB, date BIGINT);
CREATE TABLE one_users(user varchar(255));
CREATE TABLE two_users(user varchar(255));
CREATE TABLE zygzpiifxwbrrqrcvssr (messageid INTEGER PRIMARY KEY AUTOINCREMENT,user varchar(255), message LONGBLOB, date BIGINT);
CREATE TABLE zygzpiifxwbrrqrcvssr_users(user varchar(255));

```

# NOTE
	need to close all open pages before closing server with control c


# lambdas

well I would say no to this. because it is a comlie specific situation. But 
since it is a website that will be running on linux what could it hurt ?
Ubuntu also uses gcc.

[https://hackaday.com/2019/09/11/lambdas-for-c-sort-of/]()


```
#define lambda(lambda$_ret, lambda$_args, lambda$_body)\
({\
lambda$_ret lambda$__anon$ lambda$_args\
lambda$_body\
&amp;lambda$__anon$;\
})
```

This takes advantage of the gcc features I mentioned and the fact that gcc 
will let you use dollar signs in identifiers. That means that you might need 
to pass -std=gnu99 on the gcc command line to make this work properly. 
This should do it:

```sh
gcc -o clambda2 --std=gnu99 clambda2.c
```

# CAVEATS

There are caveats. You can’t depend on the lambda having a scope beyond the 
enclosing function so an asynchronous callback would not work. Accessing local
variables in the enclosing scope is iffy. Consider this code:

```c
int factor=10;
printf("%f\n",average_apply(lambda(float,(float x),{ return x/factor; })));
```

This compiles and ought to work. It does work for this online compiler. 
However, using the Linux system for Windows 10, the same code would seg fault. 
In fact, if you didn’t set the gcc -O2 option, the other examples would seg 
fault, too. On my Linux boxes, it works fine. I can’t tell if this is a bug in
Windows or if Linux is just covering up something wrong. However, it seems like
if it compiles it ought to work and — mostly — it does.

Granted, you don’t have to have lambdas with C. Even if you want them, they 
aren’t very portable. But it is still interesting to see how some gcc-specific
compiler features work and it is also an interesting intellectual exercise to 
try to match it.


# Comparisons

## `XMLHttpRequest` vs `fetch`


fetch has to use `DOMParser` for xml
```javascript
function getCurrentCity(location) {
    const lat = location.coords.latitude;
    const lon = location.coords.longitude;
    return fetch(apis.currentWeather.url(lat, lon))
        .then(response => response.text())
        .then(str => (new window.DOMParser()).parseFromString(str, "text/xml"))
        .then(data => console.log(data))
}
```

while XMLHttpRequest can set respose type to document (xml)

```javascript
const xhttp2 = new XMLHttpRequest();

xhttp2.open('GET', '/popups/addfriend.html', true);

xhttp2.responseType = 'document';

xhttp2.onload = function(){
	if(xhttp2.readyState = xhttp2.DONE && xhttp2.status == 200){

	}
}
```
