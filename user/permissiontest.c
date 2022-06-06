// Testing the uid / gid implementation and the file permissions

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void
cleanup(void)
{
  unlink("/pmtest1");
  unlink("/pmtest2");
  unlink("/pmtest3");
}

void
uidtest(void) {

  int pid, retVal;

  printf("Testing UID: ");

  if (getuid() != 0){
    printf("FAIL\n");
    return;
  }

  pid = fork();
  if (pid == -1){
    printf("FAIL\n");
    return;
  } else if (pid == 0){
    if (getuid() != 0){
      exit(-1);
    }
    setuid(1);
    if (getuid() != 1){
      exit(-1);
    } else {
      exit(0);
    }
  } else {
    wait(&retVal);
    if (retVal != 0){
      printf("FAIL\n");
      return;
    }
  }

  printf("OK\n");

}

void
gidtest(void) {

  int pid, retVal;

  printf("Testing GID: ");

  if (getgid() != 0){
    printf("FAIL\n");
    return;
  }

  pid = fork();
  if (pid == -1){
    printf("FAIL\n");
    return;
  } else if (pid == 0){
    if (getgid() != 0){
      exit(-1);
    }
    setgid(1);
    if (getgid() != 1){
      exit(-1);
    } else {
      exit(0);
    }
  } else {
    wait(&retVal);
    if (retVal != 0){
      printf("FAIL\n");
      return;
    }
  }

  printf("OK\n");

}

void
permissiontest(void) {

  int fd, pid, retVal;

  printf("Testing Permissions: ");

  fd = open("/pmtest1", O_RDWR | O_CREATE);
  if (fd < 0){
    printf("FAIL\n");
    return;
  }
  //fstat(fd, &st);
  close(fd);

  pid = fork();
  if (pid == -1){
    printf("FAIL\n");
    return;
  } else if (pid == 0){
    if ((fd = open("/pmtest1", O_RDWR)) < 0){
      exit(-1);
    }
    close(fd);
    setgid(1);
    setuid(1);
    if ((fd = open("/pmtest1", O_WRONLY)) >= 0){
      close(fd);
      exit(-1);
    }
    if ((fd = open("/pmtest1", O_RDWR)) >= 0){
      close(fd);
      exit(-1);
    }
    exit(0);
  } else {
    wait(&retVal);
    if (retVal != 0){
      printf("FAIL\n");
      return;
    }
  }

  pid = fork();
  if (pid == -1){
    printf("FAIL\n");
    return;
  } else if (pid == 0){
    setuid(1);
    if ((fd = open("/pmtest1", O_RDWR)) < 0){
      close(fd);
      exit(-1);
    }
    exit(0);
  } else {
    wait(&retVal);
    if (retVal != 0){
      printf("FAIL\n");
      return;
    }
  }

  printf("OK\n");

}

void
chowntest(void)
{
  int fd, pid, retVal;
  struct stat st;

  printf("Testing chown: ");

  fd = open("/pmtest2", O_CREATE);
  if (fd < 0){
    printf("FAIL\n");
    return;
  }
  fstat(fd, &st);
  close(fd);

  if (st.uid != 0 || st.gid != 0){
    printf("FAIL\n");
    return;
  }

  chown("/pmtest2", 1, 1);
  stat("/pmtest2", &st);

  if (st.uid != 1 || st.gid != 1){
    printf("FAIL\n");
    return;
  }

  pid = fork();
  if (pid == -1){
    printf("FAIL\n");
    return;
  } else if (pid == 0){
    setgid(2);
    setuid(2);
    chown("/pmtest2", 2, 2);
    stat("/pmtest2", &st);

    if (st.uid != 1 || st.gid != 1){
      exit(-1);
    }
    exit(0);
  } else {
    wait(&retVal);
    if (retVal != 0){
      printf("FAIL\n");
      return;
    }
  }

  printf("OK\n");

}

void
chmodtest(void)
{
  int fd, pid, retVal;
  struct stat st;

  printf("Testing chmod: ");

  fd = open("/pmtest3", O_CREATE);
  if (fd < 0){
    printf("FAIL\n");
    return;
  }
  fstat(fd, &st);
  close(fd);

  if (st.mode != 0b110110100){
    printf("FAIL\n");
    return;
  }

  chmod("/pmtest3", 0b111111101);
  stat("/pmtest3", &st);

  if (st.mode != 0b111111101){
    printf("FAIL\n");
    return;
  }

  pid = fork();
  if (pid == -1){
    printf("FAIL\n");
    return;
  } else if (pid == 0){
    setgid(1);
    setuid(1);
    chmod("/pmtest3", 0b111111111);
    stat("/pmtest3", &st);

    if (st.mode != 0b111111101){
      exit(-1);
    }
    exit(0);
  } else {
    wait(&retVal);
    if (retVal != 0){
      printf("FAIL\n");
      return;
    }
  }

  printf("OK\n");

}

int
main(void)
{
  cleanup();
  printf("PERMISSION TEST\n\n");
  uidtest();
  gidtest();
  permissiontest();
  chowntest();
  chmodtest();
  exit(0);
}