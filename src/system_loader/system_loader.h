#ifndef __SYSTEM_LOADER_H__
#define __SYSTEM_LOADER_H__

#include "../irq/irq.h"
#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE 2 * 1024 * 1024

void read_data();

#endif // __SYSTEM_LOADER_H__
