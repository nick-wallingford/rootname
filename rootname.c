#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>

#define SIZE 80

typedef struct {
  Display *dpy;
  Window root;
} rn_display;

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

static inline void init(rn_display *restrict d, rn_bat *restrict b) {
  d->dpy = XOpenDisplay(NULL);
  if (!d->dpy) {
    printf("Unable to open display.");
    exit(1);
  }
  int screen = XDefaultScreen(d->dpy);
  d->root = XRootWindow(d->dpy, screen);

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

  const int32_t battery_percentage = 100 * energy_now / b->energy_full;

  if (online) {
    return snprintf(str, size, "AC %02d%% ", battery_percentage);
  } else if (power_now) {
    const int32_t minutes = energy_now / power_now;
    const int32_t seconds = (60 * energy_now / power_now) % 60;
    return snprintf(str, size, "BAT %02d%% %d:%02d ", battery_percentage,
                    minutes, seconds);
  } else {
    return snprintf(str, size, "BAT %02d%% 0:00 ", battery_percentage);
  }
}

static size_t date(char *restrict str, size_t size) {
  const time_t t = time(NULL);
  const struct tm tm = *localtime(&t);
  return snprintf(str, size, "%04d-%02d-%02d %02d:%02d", tm.tm_year + 1900,
                  tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
}

int main() {
  rn_display d;
  rn_bat b;

  init(&d, &b);
  char name[SIZE + 1];

  for (;;) {
    name[SIZE] = 0;
    size_t size = 0;

    if (b.battery_exists)
      size += bat(&b, name + size, SIZE - size);
    size += date(name + size, SIZE - size);

    XStoreName(d.dpy, d.root, name);
    XFlush(d.dpy);
    sleep(60);
  }
}
