#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  FILE *online;
  FILE *energy_now;
  FILE *power_now;

  uint64_t energy_full;
  bool battery_exists;
} rn_bat;

void init_bat(rn_bat *restrict b);
size_t bat(rn_bat *restrict b, char *restrict str, size_t size);
