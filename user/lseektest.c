#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void
assert_offset(int fd, int offset, int whence, int expected)
{
  int actual;

  actual = lseek(fd, offset, whence);
  
  printf("Assert offset: expected %d, got %d\n", expected, actual);
  if(expected != actual){
    exit(1);
  }  
}

void
assert_value(int fd, uint8 expected)
{
  uint8 actual;

  if(read(fd, &actual, sizeof(actual)) <= 0){
    printf("read failed\n");
    exit(1);
  }
  
  printf("Assert value: expected %d, got %d\n", expected, actual);
  if(expected != actual){
    exit(1);
  }
}

void
lseektest(void)
{
  printf("lseek test\n");

  uint8 buffer[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  
  int fd_empty, fd_bytes;
  fd_empty = open("empty", O_CREATE | O_RDONLY);
  fd_bytes = open("bytes", O_CREATE | O_RDWR);

  if(fd_empty == 0 || fd_bytes == 0) {
    printf("Could not create test files\n");
    exit(1);
  }
  
  if(write(fd_bytes, buffer, sizeof(buffer)) != sizeof(buffer)){
    printf("Could not create bytes file\n");
    exit(1);
  }

  // all three whence values (with offset 0) should result in new offset 0
  assert_offset(fd_empty, 0, SEEK_SET, 0);
  assert_offset(fd_empty, 0, SEEK_CUR, 0);
  assert_offset(fd_empty, 0, SEEK_END, 0);

  // seeking outside of file boundaries should fail with return value -1
  assert_offset(fd_empty, -1, SEEK_SET, -1);
  assert_offset(fd_empty, 1, SEEK_END, -1);

  // invalid whence value should fail with return value -1
  assert_offset(fd_empty, 0, 42, -1);

  // check values of seek to first and last byte of file
  assert_offset(fd_bytes, 0, SEEK_SET, 0);
  assert_value(fd_bytes, buffer[0]);
  assert_offset(fd_bytes, -1, SEEK_END, sizeof(buffer) - 1);
  assert_value(fd_bytes, sizeof(buffer) - 1);

  close(fd_bytes);
  close(fd_empty);

  printf("OK\n");
}

int
main(int argc, char *argv[])
{
  lseektest();  
  exit(0);
}
