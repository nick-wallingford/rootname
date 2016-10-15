#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <xcb/xcb.h>

#ifdef BATTERY
#include "power.h"
#endif

#define SIZE 80

typedef struct {
  xcb_connection_t *connection;
  xcb_window_t window;
} rn_window;

static inline void init_window(rn_window *restrict window) {
  int screen_num;
  window->connection = xcb_connect(NULL, &screen_num);
  if (xcb_connection_has_error(window->connection)) {
    puts("Unable to open display.");
    xcb_disconnect(window->connection);
    exit(1);
  }

  xcb_screen_iterator_t iter =
      xcb_setup_roots_iterator(xcb_get_setup(window->connection));
  while (screen_num--)
    xcb_screen_next(&iter);

  xcb_screen_t *screen = iter.data;
  window->window = screen->root;
}

static size_t date(char *restrict str, size_t size) {
  const time_t t = time(NULL);
  const struct tm tm = *localtime(&t);
  return snprintf(str, size, "%04d-%02d-%02d %02d:%02d", tm.tm_year + 1900,
                  tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
}

int main() {
  char name[SIZE + 1];

  rn_window window;
  init_window(&window);

#ifdef BATTERY
  rn_bat battery;
  init_bat(&battery);
#endif

  for (;;) {
    name[SIZE] = 0;
    size_t size = 0;

#ifdef BATTERY
    if (battery.battery_exists)
      size += bat(&battery, name + size, SIZE - size);
#endif

    size += date(name + size, SIZE - size);

    xcb_change_property(window.connection, XCB_PROP_MODE_REPLACE, window.window,
                        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, size, name);

    if (xcb_flush(window.connection) <= 0) {
      puts("Unable to flush to X connection");
      break;
    }

    sleep(60);
  }

  xcb_disconnect(window.connection);

#ifdef BATTERY
  if (battery.battery_exists) {
    fclose(battery.online);
    fclose(battery.power_now);
    fclose(battery.energy_now);
  }
#endif

  return 2;
}
