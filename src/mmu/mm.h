#ifndef MM_H
#define MM_H

#define PAGE_MASK 0xfffffffffffff000
#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)

#define ID_MAP_PAGES 3
#define HIGH_MAP_PAGES 6
#define ID_MAP_TABLE_SIZE (ID_MAP_PAGES * PAGE_SIZE)
#define HIGH_MAP_TABLE_SIZE (HIGH_MAP_PAGES * PAGE_SIZE)
#define ENTRIES_PER_TABLE 512
#define PGD_SHIFT (PAGE_SHIFT + 3 * TABLE_SHIFT)
#define PUD_SHIFT (PAGE_SHIFT + 2 * TABLE_SHIFT)
#define PMD_SHIFT (PAGE_SHIFT + TABLE_SHIFT)
#define PUD_ENTRY_MAP_SIZE (1 << PUD_SHIFT)
#define ID_MAP_SIZE (8 * SECTION_SIZE)

#define HIGH_MAP_FIRST_START (0x0 + LINEAR_MAP_BASE)
#define HIGH_MAP_FIRST_END (0x3B400000 + LINEAR_MAP_BASE)
#define HIGH_MAP_SECOND_START (0x40000000 + LINEAR_MAP_BASE)
#define HIGH_MAP_SECOND_END (0x80000000 + LINEAR_MAP_BASE)
#define HIGH_MAP_THIRD_START (0x80000000 + LINEAR_MAP_BASE)
#define HIGH_MAP_THIRD_END (0xC0000000 + LINEAR_MAP_BASE)
#define HIGH_MAP_FOURTH_START (0xC0000000 + LINEAR_MAP_BASE)
#define HIGH_MAP_FOURTH_END (0xFC000000 + LINEAR_MAP_BASE)
#define HIGH_MAP_DEVICE_START (0xFC000000 + LINEAR_MAP_BASE)
#define HIGH_MAP_DEVICE_END (0x100000000 + LINEAR_MAP_BASE)

#define FIRST_START (0x0)
#define FIRST_END (0x3B400000)
#define SECOND_START (0x40000000)
#define SECOND_END (0x80000000)
#define THIRD_START (0x80000000)
#define THIRD_END (0xC0000000)
#define FOURTH_START (0xC0000000)
#define FOURTH_END (0xFC000000)
#define DEVICE_START (0xFC000000)
#define DEVICE_END (0x100000000)

#define MALLOC_START (0x40000000)
#define MALLOC_END (0xFC000000)
#define MALLOC_SIZE (MALLOC_END - MALLOC_START)
#define MALLOC_PAGES (MALLOC_SIZE / PAGE_SIZE)
#define THREAD_SIZE PAGE_SIZE

#define LINEAR_MAP_BASE 0xffff000000000000
#define PA_TO_KVA(pa) ((pa) + 0)   // + LINEAR_MAP_BASE
#define KVA_TO_PA(kva) ((kva) - 0) // - LINEAR_MAP_BASE

#define USER_SP_INIT_POS (2 * PAGE_SIZE)

#ifndef __ASSEMBLER__

#include "../scheduler/scheduler.h"

void memzero(uint64_t src, unsigned int n);
uint64_t allocate_user_page(struct task_struct* task, uint64_t uva);
int map_page(struct task_struct* task, uint64_t uva, uint64_t phys_page);
uint64_t map_table(uint64_t* table, uint64_t shift, uint64_t uva, int* new_table);
void map_table_entry(uint64_t* pte, uint64_t uva, uint64_t phys_page);
int copy_virt_memory(struct task_struct* dst);
int task_kp_count(struct task_struct* task);
int task_up_count(struct task_struct* task);
uint64_t get_kernel_page();
void free_kernel_page(uint64_t kp);
uint64_t get_free_page();
void free_page(uint64_t p);

extern uint64_t _start;
#define KERNEL_START (&(_start))

extern uint64_t KERNEL_PA_BASE;

#endif /* __ASSEMBLER__ */

#endif /* MM_H */
