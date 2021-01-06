# Recieving data from client

the server will recieved data from the client in __xml__ files or strings. some
of the xml data will be some publicly on the server through a route on the
server while ohter forms of __xml__ data will be sent as a string and not 
be seen public.

## Ecoding

make sure to specify the right encoding ! We don't want to accidently be in a 
encoding that could produce unexpected results.

```xml
<?xml version="1.0" encoding="UTF-8"?>
```

## Typing Elements

All Elements must have a type

Because XML is not typed. There are very simple things to make it produce the
correct types with some work on the server.

The types are the same types used in C.

<*element* type="*type*">*value*</*element*>

```xml
<age type="int">23</age>
```
