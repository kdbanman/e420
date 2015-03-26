void err_check(int ret, char *msg)
{
  if (ret == -1 || ret == NULL)
    perror(msg);
}
