#include "power.h"
#include "util.h"

void init_bat(rn_bat *restrict b) {
  FILE *energy_full = fopen("/sys/class/power_supply/BAT0/energy_full", "r");
  if (!energy_full)
    energy_full = fopen("/sys/class/power_supply/BAT0/charge_full", "r");

  if (!energy_full) {
    b->battery_exists = false;

    b->online = NULL;
    b->energy_now = NULL;
    b->power_now = NULL;
    return;
  }

  b->battery_exists = true;
  b->energy_full = read_int(energy_full);

  b->online = fopen("/sys/class/power_supply/AC/online", "r");

  b->energy_now = fopen("/sys/class/power_supply/BAT0/energy_now", "r");
  if (!b->energy_now)
    b->energy_now = fopen("/sys/class/power_supply/BAT0/charge_now", "r");

  b->power_now = fopen("/sys/class/power_supply/BAT0/power_now", "r");
  if (!b->power_now)
    b->power_now = fopen("/sys/class/power_supply/BAT0/current_now", "r");

  fclose(energy_full);
}

size_t bat(rn_bat *restrict b, char *restrict str, size_t size) {
  const bool online = read_int(b->online);
  const uint64_t energy_now = read_int(b->energy_now);
  const uint64_t power_now = read_int(b->power_now);

  const uint32_t battery_percentage = 100 * energy_now / b->energy_full;

  if (online) {
    return snprintf(str, size, "AC %02u%% ", battery_percentage);
  } else if (power_now) {
    const uint32_t minutes = energy_now / power_now;
    const uint32_t seconds = (60 * energy_now / power_now) % 60;
    return snprintf(str, size, "BAT %02u%% %d:%02u ", battery_percentage,
                    minutes, seconds);
  } else {
    return snprintf(str, size, "BAT %02u%% 0:00 ", battery_percentage);
  }
}
