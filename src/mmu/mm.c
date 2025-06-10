#include "mm.h"
#include "../io/uart/printf.h"
#include "../scheduler/scheduler.h"
#include "mmu.h"
#include <stdint.h>
#include <string.h>

uint64_t KERNEL_PA_BASE;

static uint8_t mem_map[MALLOC_PAGES] = {
	0,
};

/* returns the kva of a page that is mapped to a user process at va */
uint64_t allocate_user_page(struct task_struct* task, uint64_t uva) {
	uint64_t phys_page = get_free_page();
	// if (map_page(task, uva, phys_page) < 0)
	// 	return 0;
	return PA_TO_KVA(phys_page);
}

/* returns 0: success, <0: error */
int map_page(struct task_struct* task, uint64_t uva, uint64_t phys_page) {
	if (!task->mm.pgd) {
		int kp_count = task_kp_count(task);
		if (kp_count == MAX_PAGE_COUNT)
			return -1;
		task->mm.pgd = get_free_page();
		task->mm.kernel_pages[kp_count] = task->mm.pgd;
	}
	uint64_t pgd = task->mm.pgd;
	int new_table;

	uint64_t pud = map_table((uint64_t*)PA_TO_KVA(pgd), PGD_SHIFT, uva, &new_table);
	if (new_table) {
		int kp_count = task_kp_count(task);
		if (kp_count == MAX_PAGE_COUNT) {
			free_page(pud);
			return -1;
		}
		task->mm.kernel_pages[kp_count] = pud;
	}

	uint64_t pmd = map_table((uint64_t*)PA_TO_KVA(pud), PUD_SHIFT, uva, &new_table);
	if (new_table) {
		int kp_count = task_kp_count(task);
		if (kp_count == MAX_PAGE_COUNT) {
			free_page(pmd);
			return -1;
		}
		task->mm.kernel_pages[kp_count] = pmd;
	}

	uint64_t pte = map_table((uint64_t*)PA_TO_KVA(pmd), PMD_SHIFT, uva, &new_table);
	if (new_table) {
		int kp_count = task_kp_count(task);
		if (kp_count == MAX_PAGE_COUNT) {
			free_page(pte);
			return -1;
		}
		task->mm.kernel_pages[kp_count] = pte;
	}

	map_table_entry((uint64_t*)PA_TO_KVA(pte), uva, phys_page);
	int up_count = task_up_count(task);
	if (up_count == MAX_PAGE_COUNT)
		return -1;
	struct user_page up = {phys_page, uva};
	task->mm.user_pages[up_count] = up;
	return 0;
}

/*
 * args:
 * @table: kva of table to fill an entry
 * @shift: shift used to turn va into index
 * @va: the va we want to map
 * @new_table: whether the entry is already filled and the next level allocated already
 *
 * returns: pa of the next level
 */
uint64_t map_table(uint64_t* table, uint64_t shift, uint64_t uva, int* new_table) {
	uint64_t index = uva >> shift;
	index = index & (ENTRIES_PER_TABLE - 1);
	if (!table[index]) {
		*new_table = 1;
		uint64_t next_level_table = get_free_page();
		uint64_t entry = next_level_table | TD_USER_TABLE_FLAGS;
		table[index] = entry;
		return next_level_table;
	} else {
		*new_table = 0;
		return table[index] & PAGE_MASK;
	}
}

void map_table_entry(uint64_t* pte, uint64_t uva, uint64_t phys_page) {
	uint64_t index = uva >> PAGE_SHIFT;
	index = index & (ENTRIES_PER_TABLE - 1);
	uint64_t entry = phys_page | TD_USER_PAGE_FLAGS;
	pte[index] = entry;
}

/* copy current's memory space to dst */
int copy_virt_memory(struct task_struct* dst) {
	for (int i = 0; i < task_up_count(current); i++) {
		uint64_t kva = allocate_user_page(dst, current->mm.user_pages[i].uva);
		if (kva == 0)
			return -1;
		memcpy((uint64_t*)kva, (uint64_t*)(current->mm.user_pages[i].uva), PAGE_SIZE);
	}
	return 0;
}

/* utility function to get used count of kernel pages array */
int task_kp_count(struct task_struct* task) {
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		if (task->mm.kernel_pages[i] == 0)
			return i;
	}
	return MAX_PAGE_COUNT;
}

/* utility function to get used count of user pages array */
int task_up_count(struct task_struct* task) {
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		if (task->mm.user_pages[i].pa == 0)
			return i;
	}
	return MAX_PAGE_COUNT;
}

/* returns a kva of a free page */
uint64_t get_kernel_page() {
	uint64_t phys_page = get_free_page();
	return PA_TO_KVA(phys_page);
}

void free_kernel_page(uint64_t kp) {
	free_page(KVA_TO_PA(kp));
}

/* returns a pa of a free page, does not return if it fails */
uint64_t get_free_page() {
	for (int i = 0; i < MALLOC_PAGES; i++) {
		if (mem_map[i] == 0) {
			mem_map[i] = 1;
			uint64_t ret = MALLOC_START + i * PAGE_SIZE;
			memzero(PA_TO_KVA(ret), PAGE_SIZE);
			return ret;
		}
	}
	puts("Out of physical memory!\n");
	/* doesn't comeback */
	return 0;
}

void free_page(uint64_t p) {
	mem_map[(p - MALLOC_START) / PAGE_SIZE] = 0;
}

int do_data_abort(uint64_t far, uint64_t esr) {
	uint64_t dfsc = esr & 0x3f;
	/* 0x7: level 3 translation fault */
	if (dfsc == 0x7) {
		uint64_t page = get_free_page();
		if (map_page(current, far & PAGE_MASK, page) < 0)
			return -1;
		return 0;
	}
	return -1;
}
