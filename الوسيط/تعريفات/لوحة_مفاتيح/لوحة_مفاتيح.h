#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "بوابة_النظام.h"

#define KEY_NONE   0

#define KEY_UP     1001
#define KEY_DOWN   1002
#define KEY_LEFT   1003
#define KEY_RIGHT  1004

void keyboard_run(kernel_api_t* api);

#endif
