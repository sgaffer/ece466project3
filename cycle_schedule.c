#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern inst_t instList;
extern int count;

void cycle_schedule(ddg_t ddg) {

    //int cycle = 0;
    int i, j;
    int swapped;
    int number_of_instructions = count - 1;
    inst_t list = instList;
    //inst_t ordered_list_head = instList;
    //inst_t ordered_list_tail;
    //inst_t temp = instList;
    inst_t temp;
    inst_t *inst_list;
    int min_index;
    int max_index;
    
    inst_list = (inst_t*) malloc(number_of_instructions * sizeof (inst_t));

    for (i = 0; i < number_of_instructions; inst_list[i++] = NULL); // initialize list of instructions to NULL

    while (list) {
        inst_list[list->count] = list;

        list = list->next;
    }

    ddg.ready_cycle = (int *) malloc(number_of_instructions * sizeof (int));
    ddg.schedule_time = (int *) malloc(number_of_instructions * sizeof (int));

    for (i = 0; i < number_of_instructions; i++) {
        ddg.ready_cycle[i] = 0;
        ddg.schedule_time[i] = -1;
    }

    for (min_index = 0; inst_list[min_index] == NULL; min_index++);

    for (max_index = min_index; inst_list[max_index]->op != OP_RET; max_index++);
    //max_index--;

    //printf("min index = %d, max index = %d\n", min_index, max_index);

    j = 1;
    while (swapped) {
        swapped = 0;
        j++;
        for (i = min_index; i < (max_index - j); i++) {
            if (inst_list[i + 1]->depth < inst_list[i]->depth) {
                temp = inst_list[i];
                inst_list[i] = inst_list[i + 1];
                inst_list[i + 1] = temp;
                swapped=1;
            }
        }
    }
    
    for (i = min_index; i < (max_index - 1); i++) {
        inst_list[i]->next = inst_list[i+1];
    }
    inst_list[max_index]->next = NULL;

    
    
    /*
    while(list) {
        if (ddg.ready_cycle[list->count] <= cycle) {
            //&& (all dependents on X are scheduled)
            //&& (all resources for X are available))
            if (ddg.flow_arc[list->count])
        
        }
        
        list = list->next;
    }
     */
    return;
}
