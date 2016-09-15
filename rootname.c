#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <xcb/xcb.h>

#define SIZE 80

typedef struct {
  xcb_connection_t *connection;
  xcb_window_t *window;
} rn_window;

typedef struct {
  FILE *online;
  FILE *energy_now;
  FILE *power_now;

  uint64_t energy_full;
  bool battery_exists;
} rn_bat;

static uint64_t read_int(FILE *restrict f) {
  uint64_t ret;
  char buffer[SIZE];

  rewind(f);
  size_t bytes = fread(buffer, 1, SIZE, f);
  buffer[bytes] = 0;

  sscanf(buffer, "%" SCNu64, &ret);
  return ret;
}

static inline void init_window(rn_window *restrict window) {
  window->connection = xcb_connect(NULL, NULL);
  if (xcb_connection_has_error(window->connection)) {
    puts("Unable to open display.");
    xcb_disconnect(window->connection);
    exit(1);
  }

  xcb_screen_iterator_t iter =
      xcb_setup_roots_iterator(xcb_get_setup(window->connection));
  window->window = iter.data;
}

static inline void init_bat(rn_bat *restrict b) {
  FILE *energy_full = fopen("/sys/class/power_supply/BAT0/energy_full", "r");
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
  b->power_now = fopen("/sys/class/power_supply/BAT0/power_now", "r");

  fclose(energy_full);
}

static size_t bat(rn_bat *restrict b, char *restrict str, size_t size) {
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

static size_t date(char *restrict str, size_t size) {
  const time_t t = time(NULL);
  const struct tm tm = *localtime(&t);
  return snprintf(str, size, "%04d-%02d-%02d %02d:%02d", tm.tm_year + 1900,
                  tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
}

int main() {
  rn_window window;
  rn_bat battery;
  char name[SIZE + 1];

  init_window(&window);
  init_bat(&battery);

  for (;;) {
    name[SIZE] = 0;
    size_t size = 0;

    if (battery.battery_exists)
      size += bat(&battery, name + size, SIZE - size);
    size += date(name + size, SIZE - size);

    xcb_change_property(window.connection, XCB_PROP_MODE_REPLACE,
                        *window.window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                        size, name);

    if (xcb_flush(window.connection) <= 0) {
      puts("Unable to flush to X connection");
      break;
    }

    sleep(60);
  }

  xcb_disconnect(window.connection);
  if (battery.battery_exists) {
    fclose(battery.online);
    fclose(battery.power_now);
    fclose(battery.energy_now);
  }

  return 2;
}
