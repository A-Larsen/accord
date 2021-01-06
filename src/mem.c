#include "mem.h"

void **
dynamicArrayResize(data, nmemb, size)
	void **data;
	size_t nmemb;
	size_t size;
{
	data = 
		nmemb == 1 ? 
		malloc(size) : 
		realloc(data, size * nmemb);

	data[nmemb-1] = malloc(size);
	return data;
}

void *
dynamicArrayResize2(data, nmemb, size)
	void *data;
	size_t nmemb;
	size_t size;
{
	/* data = */ 
	/* 	nmemb <= 0 ? */ 
	/* 	malloc(size) : */ 
	/* 	realloc(data, size * nmemb); */

	/* data[nmemb-1] = malloc(size); */
	/* data = */
	/* 	nmemb <= 0 ? */
	/* 	malloc(size): */
	/* 	realloc(data, size * nmemb); */
	data =
		nmemb == 1 ?
		malloc(size):
		realloc(data, size * nmemb);
	return data;
}
