#include "convert.h"

char *
ts_to_readable(ts)
  long long int ts;
{
		time_t c = (time_t)ts;
		char *TIME = ctime( &c );
		TIME[strlen(TIME)-1] = 0;

		return TIME;
}

