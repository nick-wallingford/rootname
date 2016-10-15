#include "util.h"

#define SIZE 40

uint64_t read_int(FILE *restrict f) {
  uint64_t ret;
  char buffer[SIZE];

  rewind(f);
  size_t bytes = fread(buffer, 1, SIZE, f);
  buffer[bytes] = 0;

  sscanf(buffer, "%" SCNu64, &ret);
  return ret;
}

