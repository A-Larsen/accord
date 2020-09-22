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

I don't want users to be able to send messages throught the console

