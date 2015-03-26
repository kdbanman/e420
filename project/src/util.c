
#include <errno.h>
#include <stdio.h>

void err_check(void *ret, char *msg)
{
  long ret_int = (long) ret;
  if (ret_int == -1 || ret_int == 0)
    perror(msg);
}
