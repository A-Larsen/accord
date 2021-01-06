# ~~ISSUE 1~~

you can send a message when no chatrooms are selected. If you do so you get a 
segfault

if you click send on a popup this problem doesn't seem to occur

# ~~ISSUE 2~~
going to

```
0.0.0.0:8080/login/data
```

causes a crash

# ISSUE 3

in a browser console I can do something like

```javascript
> ws.send("<root><user closing=\"false\" id=\"73\"></user><message chatroom=\"ktclilwwceccxmjcqxzc\" date=\"1600794987\">not good&lt;br&gt;</message></root>")
```

I don't want users to be able to send messages throught the console.

this also means that you could send a message as a different user than the one 
you are logged into

```javascript
> ws.send('<root><user closing="false" id="24"></user><message chatroom="lnohzaevmzswqfsqucbj" date="1600796468"></message></root>')
```

a blank message could cause a crash on the server side. I need bounds checking
in parseXML on server side

I'm going to set js to null on the last line so that the websocket is not 
accessable

```javascript
ws = null
```
.. this doesn't work because the socket needs to stay open for messages and 
adding rooms and friends

# ISSUE 4

logging in as user one and then logging into user two.
if you proceed to close user ones page then user two 
cannot send messages without a page refresh.

I belive changing how new websockets are chreated in
`server_connection_chat()` could fix this.

```c
// we have to make a ne websocket for RELOGGIN
// lets not have that
USERS[usercount]->ws = onion_websocket_new(req, res); 
```
