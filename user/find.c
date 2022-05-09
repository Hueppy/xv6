#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void
find(char *path, char *pattern)
{
  int fd;
  char buf[512], *p;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open current directory\n");
    return;
  }
  strcpy(buf, path);
  p = buf+strlen(buf);
  *p++ = '/';
  
  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0 || de.name[0] == '.')
      continue;
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if (stat(buf, &st) < 0){
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }
    if (strstr(p, pattern) != 0)
      printf("%d\t%s\n", st.type, buf);
    if (st.type == T_DIR)
      find(buf, pattern);
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  while(*(++argv) != 0){
    find(".", *argv);
  }
  exit(0);
}
