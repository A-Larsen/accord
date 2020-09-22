# ISSUE 1

you can send a message when no chatrooms are selected. If you do so you get a 
segfault

if you click send on a popup this problem doesn't seem to occur

# ISSUE 2
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
