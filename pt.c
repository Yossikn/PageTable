#include <stdlib.h>
#include <stdio.h>
#include "os.h"

uint64_t* get_PTE(uint64_t virtual_page, uint64_t node, int level){
    uint64_t entry = (virtual_page >> ((4 - level) * 9) & 0x1ff);
    uint64_t* PTE = (uint64_t*) phys_to_virt(node);
    return (entry + PTE);
}

int is_PTE_invalid(uint64_t PTE){
    return !(PTE & 1);
}

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn){
    uint64_t page_table = pt << 12;

    for (int i = 0; i < 5; ++i) {
        uint64_t* PTE = get_PTE(vpn, page_table, i);
        if (is_PTE_invalid(*PTE)){
            if (ppn == NO_MAPPING) {
                return;
            }
            if (i != 4) {
                *PTE = (alloc_page_frame() << 12) + 1;
            }
        }
        if (i == 4){
            if (ppn == NO_MAPPING) {
                *PTE = 0;
                return;
            }
            *PTE = (ppn << 12) + 1;
        }
        page_table = (*PTE >> 12) << 12;
    }
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn){
    uint64_t page_table = pt << 12;
    for (int i = 0; i < 5; ++i) {
        uint64_t *PTE = get_PTE(vpn, page_table, i);
        if (is_PTE_invalid(*PTE)) {
            return NO_MAPPING;
            }
        if (i == 4){
            return (*PTE >> 12);
        }
        page_table = (*PTE >> 12) << 12;
    }
    return NO_MAPPING;
}

