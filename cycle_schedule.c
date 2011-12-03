#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern inst_t instList;
extern int count;

void sort_by_cycle(ddg_t *ddg, inst_t *inst_list) {

    inst_t list = instList;
    inst_t temp;
    int i, j, min_index, max_index;
    int swapped = 1;
    int block;
    int block_start;

    while (list) {
        inst_list[list->count] = list;
        list = list->next;
    }

    for (min_index = 0; inst_list[min_index] == NULL; min_index++);
    max_index = count - 1;

    block = min_index + 1;
    block_start = min_index;
    while (block <= count) {
        if (block != count)
            do {
                if (inst_list[block]->label != NULL)
                    break;
                block++;
            } while (block != count);
        for (i = block_start; i < block; i++) {
            swapped = 0;
            for (j = block_start; j < block - (i - block_start + 1); j++) {
                if (ddg->schedule_time[inst_list[j + 1]->count] < ddg->schedule_time[inst_list[j]->count]) {
                    if (inst_list[j + 1]->label) {
                        inst_list[j]->label = inst_list[j + 1]->label;
                        inst_list[j + 1]->label = NULL;
                    } else if (inst_list[j]->label) {
                        inst_list[j + 1]->label = inst_list[j]->label;
                        inst_list[j]->label = NULL;
                    }
                    temp = inst_list[j];
                    inst_list[j] = inst_list[j + 1];
                    inst_list[j + 1] = temp;
                    swapped = 1;
                }
            }
            if (!swapped)
                break;
        }
        block_start = block;
        block++;
    }

    for (i = min_index; i < max_index; i++) {
        inst_list[i]->next = inst_list[i + 1];
    }
    inst_list[max_index]->next = NULL;

    instList = inst_list[min_index];

    /*list = instList;
    while (list) {
        if (list->label)
            printf("%s ", list->label);
        printf("%d\n", ddg->schedule_time[list->count]);
        list = list->next;
    }*/

    return;
}

inst_t *sort_by_depth() {
    inst_t *inst_list;
    inst_t list = instList;
    inst_t temp;
    int i, j, min_index, max_index;
    int swapped = 1;
    int block_end, block_start;

    inst_list = (inst_t*) malloc(count * sizeof (inst_t));

    for (i = 0; i < count; inst_list[i++] = NULL); // initialize list of instructions to NULL

    while (list) {
        inst_list[list->count] = list;
        list = list->next;
    }

    for (min_index = 0; inst_list[min_index] == NULL; min_index++);
    max_index = count - 1;
    block_end = min_index + 1;
    block_start = min_index;

    while (block_end <= count) {
        if (block_end != count)
            do {
                if (inst_list[block_end]->label != NULL)
                    break;
                block_end++;
            } while (block_end != count);
        for (i = block_start; i < block_end; i++) {
            swapped = 0;
            for (j = block_start; j < block_end - (i - block_start + 1); j++) {
                if (inst_list[j + 1]->depth < inst_list[j]->depth) {
                    if (inst_list[j + 1]->label) {
                        inst_list[j]->label = inst_list[j + 1]->label;
                        inst_list[j + 1]->label = NULL;
                    } else if (inst_list[j]->label) {
                        inst_list[j + 1]->label = inst_list[j]->label;
                        inst_list[j]->label = NULL;
                    }
                    temp = inst_list[j];
                    inst_list[j] = inst_list[j + 1];
                    inst_list[j + 1] = temp;
                    swapped = 1;
                }
            }
            if (!swapped)
                break;
        }
        block_start = block_end;
        block_end++;
    }

    for (i = min_index; i < max_index; i++) {
        inst_list[i]->next = inst_list[i + 1];
    }
    inst_list[max_index]->next = NULL;

    instList = inst_list[min_index];

    return inst_list;
}

void cycle_schedule(inst_t *inst_list, ddg_t *ddg, int slots, int min_index, int max_index) {

    int cycle;
    int i, j, k;
    inst_t X, Y, Z;
    int deps_met = 0;
    int used_slots = 0;
    int ops_in_list = 1;

    cycle = 0; // 4
    while (ops_in_list != 0) { // 5
        for (i = min_index; i <= max_index; i++) { // 7
            X = inst_list[i]; // 9
            if (ddg->ready_cycle[X->count] <= cycle) {
                for (j = i; j <= max_index; j++) {
                    Z = inst_list[j];
                    if (ddg->flow_arc[Z->count][X->count] == 1) {
                        deps_met = 0;
                        break;
                    } else if (ddg->output_arc[Z->count][X->count] == 1) {
                        deps_met = 0;
                        break;
                    } else
                        deps_met = 1;
                }
                if (deps_met == 1 && used_slots < slots) { // 11
                    if (i < max_index) {
                        k = i + 1;
                        for (Y = inst_list[k]; k <= max_index; Y = inst_list[++k]) {
                            if (ddg->flow_arc[X->count][Y->count] == 1) // 15
                                ddg->ready_cycle[Y->count] = max(ddg->ready_cycle[Y->count], cycle + latency(X)); // 16
                            else if (ddg->anti_arc[X->count][Y->count] == 1) // 17
                                ddg->ready_cycle[Y->count] = max(ddg->ready_cycle[Y->count], cycle); // 18
                            else if (ddg->output_arc[X->count][Y->count] == 1) // 19
                                ddg->ready_cycle[Y->count] = max(ddg->ready_cycle[Y->count], cycle + max(0, latency(X) - latency(Y) + 1)); // 20 
                        }
                    }
                    ddg->schedule_time[X->count] = cycle; // 23
                    used_slots++; // resource used
                    k = i; // k is current instruction's position on list

                    do { // shift entire list down to delete instruction
                        inst_list[k] = inst_list[k + 1];
                        k++;
                    } while (k < max_index);
                    max_index--; // list is one unit smaller now
                }
            }
        }
        cycle++;
        used_slots = 0;
        if (max_index < min_index) {
            ops_in_list = 0;
        } else
            ops_in_list = 1;
    }

    return;
}


