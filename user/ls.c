#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define PERMISSION_LENGTH 9

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
fmtmode(short mode, char *perm){
  char *template = "rwxrwxrwx";
  int mask;

  mask = 1 << (PERMISSION_LENGTH - 1);
  while(mask > 0){
    if(mode & mask){
      *perm = *template;
    } else {
      *perm = '-';
    }
    
    perm++;
    template++;
    mask >>= 1;
  }
}

void
ls(char *path)
{
  char buf[512], *p, mode[PERMISSION_LENGTH + 1];
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    mode[0] = '-';
    fmtmode(st.mode, mode + 1);
    printf("%s %d %d %d %l %s\n", mode, st.nlink, st.uid, st.gid, st.size, fmtname(path));
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }

      if(st.type == T_DIR)
        mode[0] = 'd';
      else if(st.type == T_SYMLINK)
        mode[0] = 'l';
      else
        mode[0] = '-';
      fmtmode(st.mode, mode + 1);
      printf("%s %d %d %d %l %s\n", mode, st.nlink, st.uid, st.gid, st.size, fmtname(buf));
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
