#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * Battery statistics struct. Allows persistant access to power/battery status
 * information without having to create a new file descriptor.
 */
typedef struct {
  /** This file allows the program to determine whether the machine is currently
   * plugged in, vs on battery power. */
  FILE *online;

  /** This file indicates how full the battery is. */
  FILE *energy_now;

  /** This file indicates how quickly the battery is being drained currently. */
  FILE *power_now;

  /** Maximum capacity of the battery. */
  uint64_t energy_full;

  /** Whether a battery actually exists. bat() will do very little if this is
   * false. */
  bool battery_exists;
} rn_bat;

/**
 * Initializes an rn_bat struct.
 *
 * @param b A pointer to the rn_bat struct to intialize.
 */
void init_bat(rn_bat *restrict b);

/**
 * Print current power statistics to a string.
 *
 * @param b An already initialized battery descriptor.
 * @param str A string to print power statistics to.
 * @param size Maximum number of bytes, including the terminating '\0', to write
 * the string.
 * @return The number of bytes, including the terminating '\0', written to the
 * string.
 */
size_t bat(rn_bat *restrict b, char *restrict str, size_t size);

/**
 * De-initializes an rn_bat struct.
 *
 * @param b A pointer to the rn_bat struct to de-initialize.
 */
void close_bat(rn_bat *restrict b);
