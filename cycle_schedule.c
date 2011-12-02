#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern inst_t instList;
extern int count;

inst_t *sort_by_depth() {
    inst_t *inst_list;
    inst_t list = instList;
    inst_t temp;
    int i, j, min_index, max_index;
    int swapped = 1;

    inst_list = (inst_t*) malloc(count * sizeof (inst_t));

    for (i = 0; i < count; inst_list[i++] = NULL); // initialize list of instructions to NULL

    while (list) {
        inst_list[list->count] = list;
        list = list->next;
    }

    for (min_index = 0; inst_list[min_index] == NULL; min_index++);
    for (max_index = min_index; inst_list[max_index]->op != OP_RET; max_index++);

    j = 1;
    while (swapped) {
        swapped = 0;
        j++;
        for (i = min_index; i < (max_index - j); i++) {
            if (inst_list[i + 1]->depth < inst_list[i]->depth) {
                temp = inst_list[i];
                inst_list[i] = inst_list[i + 1];
                inst_list[i + 1] = temp;
                swapped = 1;
            }
        }
    }

    for (i = min_index; i < (max_index - 1); i++) {
        inst_list[i]->next = inst_list[i + 1];
    }
    inst_list[max_index]->next = NULL;

    return inst_list;
}

void cycle_schedule(inst_t *inst_list, ddg_t ddg, int slots) {

    int cycle;
    int i, j, k;
#ifdef debug
    inst_t list = instList;
#endif
    int min_index, max_index;
    inst_t X, Y;
    int deps_met = 0;
    int used_slots = 0;
    int ops_in_list = 1;
#ifdef debug
    int total_removed = 0;
#endif

    ddg.ready_cycle = (int *) malloc(count * sizeof (int));
    ddg.schedule_time = (int *) malloc(count * sizeof (int));

    for (min_index = 0; inst_list[min_index] == NULL; min_index++);
    for (max_index = min_index; inst_list[max_index]->op != OP_RET; max_index++);

    for (i = 0; i < count; i++) {
        ddg.schedule_time[i] = -1;
    }

    for (i = 0; i < count; ddg.ready_cycle[i++] = 0); // 3
    cycle = 0; // 4
    while (ops_in_list != 0) { // 5
        for (i = min_index; i <= max_index; i++) { // 7
            X = inst_list[i]; // 9
            if (ddg.ready_cycle[X->count] <= cycle) {
                for (j = min_index; j <= max_index; j++) {
                    if (ddg.flow_arc[X->count][inst_list[j]->count] == 1) {
                        deps_met = 0;
                        break;
                    } else if (ddg.output_arc[X->count][inst_list[j]->count] == 1) {
                        deps_met = 0;
                        break;
                    } else
                        deps_met = 1;
                }
                if (deps_met == 1 && used_slots < slots) { // 11
                    if (X->next != NULL) {
                        for (Y = X->next; Y->next != NULL; Y = Y->next) {
                            if (ddg.flow_arc[Y->count][X->count] == 1)
                                ddg.ready_cycle[Y->count] = max(ddg.ready_cycle[Y->count], cycle + latency(X));
                            else if (ddg.anti_arc[Y->count][X->count] == 1)
                                ddg.ready_cycle[Y->count] = max(ddg.ready_cycle[Y->count], cycle);
                            else if (ddg.output_arc[Y->count][X->count] == 1)
                                ddg.ready_cycle[Y->count] = max(ddg.ready_cycle[Y->count], cycle + max(0, latency(X) - latency(Y) + 1));
                        }
                    }
                    ddg.schedule_time[X->count] = cycle;
                    used_slots++;
                    k = i;
#ifdef debug
                    total_removed++;
                    printf("%d\n", inst_list[k]->count);
#endif
                    do {
                        inst_list[k] = inst_list[k + 1];
                        k++;
                    } while (k < max_index);
                    max_index--;
                }
            }
        }
        cycle++;
        used_slots = 0;
        if (max_index < min_index) {
#ifdef debug
            //printf("%d still in list!\n", j);
#endif
            ops_in_list = 0;
        } else
            ops_in_list = 1;
    }

#ifdef debug
    printf("----------------------------------------\n");
    for (list = instList; list->next != NULL; list = list->next) {
        printf("count = %d, schedule_time = %d\n", list->count, ddg.schedule_time[list->count]);
    }
#endif

    return;
}
