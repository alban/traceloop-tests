#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


char *big_buffer;
#define FILENAME "/tmp/test1.txt"
char *filename1;
char *filename2;

int main() {
  int fd;

  printf("pid\n%d\n", getpid());

  big_buffer = mmap((void*)0x05000000, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  memset(big_buffer, 0, sizeof(4096));

  filename1 = big_buffer;
  strcpy(filename1, FILENAME);
  filename2 = big_buffer + 4096 - strlen(FILENAME) - 2;
  strcpy(filename2, FILENAME);

  printf("filename1: %s\n", filename1);
  printf("filename2: %s\n", filename2);

  sleep(1);

  fd = open(filename1, O_RDONLY, 0);
  printf("open %s: fd %d\n", filename1, fd);
  close(fd);

  fd = open(filename2, O_RDONLY, 0);
  printf("open %s: fd %d\n", filename2, fd);
  close(fd);

  sleep(1);
  return 0;
}

