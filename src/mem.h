#ifndef _MEM_
#define _MEM_
#include <malloc.h>

void ** dynamicArrayResize(void **data, size_t nmemb, size_t size);
void * dynamicArrayResize2(void *data, size_t nmemb, size_t size);

#endif 
