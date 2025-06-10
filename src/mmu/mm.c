#include "mm.h"
#include "mmu.h"
#include <string.h>

static uint8_t mem_map[PAGING_PAGES] = {
	0,
};

uint64_t allocate_kernel_page() {
	uint64_t page = get_free_page();
	if (page == 0) {
		return 0;
	}
	return page + VA_START;
}

uint64_t allocate_user_page(struct task_struct* task, uint64_t va) {
	uint64_t page = get_free_page();
	if (page == 0) {
		return 0;
	}
	map_page(task, va, page);
	return page + VA_START;
}

uint64_t get_free_page() {
	for (int i = 0; i < PAGING_PAGES; i++) {
		if (mem_map[i] == 0) {
			mem_map[i] = 1;
			uint64_t page = LOW_MEMORY + i * PAGE_SIZE;
			memset((void*)(page + VA_START), 0, PAGE_SIZE);
			return page;
		}
	}
	return 0;
}

void free_page(uint64_t p) {
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}

void map_table_entry(uint64_t* pte, uint64_t va, uint64_t pa) {
	uint64_t index = va >> PAGE_SHIFT;
	index = index & (PTRS_PER_TABLE - 1);
	uint64_t entry = pa | MMU_PTE_FLAGS;
	pte[index] = entry;
}

uint64_t map_table(uint64_t* table, uint64_t shift, uint64_t va, int* new_table) {
	uint64_t index = va >> shift;
	index = index & (PTRS_PER_TABLE - 1);
	if (!table[index]) {
		*new_table = 1;
		uint64_t next_level_table = get_free_page();
		uint64_t entry = next_level_table | MM_TYPE_PAGE_TABLE;
		table[index] = entry;
		return next_level_table;
	} else {
		*new_table = 0;
	}
	return table[index] & PAGE_MASK;
}

void map_page(struct task_struct* task, uint64_t va, uint64_t page) {
	uint64_t pgd;
	if (!task->mm.pgd) {
		task->mm.pgd = get_free_page();
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = task->mm.pgd;
	}
	pgd = task->mm.pgd;
	int new_table;
	uint64_t pud = map_table((uint64_t*)(pgd + VA_START), PGD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pud;
	}
	uint64_t pmd = map_table((uint64_t*)(pud + VA_START), PUD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pmd;
	}
	uint64_t pte = map_table((uint64_t*)(pmd + VA_START), PMD_SHIFT, va, &new_table);
	if (new_table) {
		task->mm.kernel_pages[++task->mm.kernel_pages_count] = pte;
	}
	map_table_entry((uint64_t*)(pte + VA_START), va, page);
	struct user_page p = {page, va};
	task->mm.user_pages[task->mm.user_pages_count++] = p;
}

int32_t copy_virt_memory(struct task_struct* dst) {
	struct task_struct* src = current;
	for (int i = 0; i < src->mm.user_pages_count; i++) {
		uint64_t kernel_va = allocate_user_page(dst, src->mm.user_pages[i].virt_addr);
		if (kernel_va == 0) {
			return -1;
		}
		memcpy((void*)kernel_va, (void*)src->mm.user_pages[i].virt_addr, PAGE_SIZE);
	}
	return 0;
}

static int32_t ind = 1;

int32_t do_mem_abort(uint64_t addr, uint64_t esr) {
	uint64_t dfs = (esr & 0b111111);
	if ((dfs & 0b111100) == 0b100) {
		uint64_t page = get_free_page();
		if (page == 0) {
			return -1;
		}
		map_page(current, addr & PAGE_MASK, page);
		ind++;
		if (ind > 2) {
			return -1;
		}
		return 0;
	}
	return -1;
}
