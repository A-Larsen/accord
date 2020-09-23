
# Seting cookies

```c
onion_dict *h = onion_response_get_headers(res);

/* onion_response_add_cookie(res, "test", "true", -1, */ 
/* 							"/chat", "0.0.0.0", OC_HTTP_ONLY | OC_SECURE); */
onion_response_add_cookie(res, "test", "true", -1, 
							/* "/", "0.0.0.0", OC_HTTP_ONLY | OC_SECURE); */
							/* "/", "0.0.0.0", OC_HTTP_ONLY); */
							"/", "0.0.0.0", 0);


const char *dict = onion_dict_get(h, "Set-Cookie");
/* const char *dict = onion_dict_get(h, "Content-Type"); */
ONION_INFO("DICT: %s\n", dict);
```
