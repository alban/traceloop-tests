#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

int main() {
#define FILENAME "/tmp/traceloop-test.data"

  open(FILENAME, O_RDWR, 0);
  open(FILENAME, O_RDWR, 0);
  return 0;
}

