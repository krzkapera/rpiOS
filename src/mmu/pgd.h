#ifndef __PGD_H__
#define __PGD_H__

#include <stdint.h>

extern void set_pgd(uint64_t pgd);
extern uint64_t get_pgd();

#endif // __PGD_H__
