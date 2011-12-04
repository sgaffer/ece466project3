#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern inst_t instList;

int max(int first, int second) {
    if (first > second)
        return first;
    else
        return second;
}

int number_of_registers() {
    int biggest_reg = 0;
    inst_t list = instList;
    int i;

    while (list) {
        for (i = 0; i <= 2; i++) {
            if (list->ops[i].t == op_reg)
                biggest_reg = max(biggest_reg, list->ops[i].reg);
        }
        list = list->next;
    }
    return biggest_reg + 1;
}

int latency(inst_t list) {
    if (list->op == OP_LD || list->op == OP_LDR)
        return 2;
    else if (list->op == OP_MUL)
        return 3;
    else
        return 1;
}

void calc_depth(inst_t *inst_list, int min_index, int max_index) {

    int MAX_REGS;
    int *latest_use_time, *completion_time;
    int t, j;
    int i;
    int max_depth = 0;

    MAX_REGS = number_of_registers();

    latest_use_time = (int *) malloc(MAX_REGS * sizeof (int));
    completion_time = (int *) malloc(MAX_REGS * sizeof (int));

    for (i = 0; i < MAX_REGS; i++) {
        latest_use_time[i] = 0;
        completion_time[i] = 0;
    }

    while (min_index <= max_index) {
        if (inst_list[min_index]->ops[0].t == op_reg && inst_list[min_index]->op != OP_STR && inst_list[min_index]->op != OP_BR) {
            t = max(1, latest_use_time[inst_list[min_index]->ops[0].reg] - latency(inst_list[min_index]) + 1);
        }
        if (inst_list[min_index]->op == OP_IN) { // IN writes to R0
            t = max(1, latest_use_time[0] - latency(inst_list[min_index]) + 1);
        }
        for (j = 1; j <= 2; j++) {
            if (inst_list[min_index]->ops[j].t == op_reg) {
                t = max(t, completion_time[inst_list[min_index]->ops[j].reg]);
            }
        }
        if (inst_list[min_index]->op == OP_STR || inst_list[min_index]->op == OP_BR) { // STR reads from first register
            t = max(t, completion_time[inst_list[min_index]->ops[0].reg]);
        }
        if (inst_list[min_index]->op == OP_OUT) { // OUT reads from R0
            t = max(t, completion_time[0]);
        }
        if (inst_list[min_index]->ops[0].t == op_reg && inst_list[min_index]->op != OP_STR && inst_list[min_index]->op != OP_BR) {
            t = max(t, completion_time[inst_list[min_index]->ops[0].reg] - latency(inst_list[min_index]));
        }
        if (inst_list[min_index]->op == OP_IN) { // IN writes to R0
            t = max(t, completion_time[0] - latency(inst_list[min_index]));
        }
        if (inst_list[min_index]->ops[0].t == op_reg && inst_list[min_index]->op != OP_STR && inst_list[min_index]->op != OP_BR) {
            completion_time[inst_list[min_index]->ops[0].reg] = t + latency(inst_list[min_index]);
        }
        if (inst_list[min_index]->op == OP_IN) { // IN writes to R0
            completion_time[0] = t + latency(inst_list[min_index]);
        }
        for (j = 1; j <= 2; j++) {
            if (inst_list[min_index]->ops[j].t == op_reg) {
                latest_use_time[inst_list[min_index]->ops[j].reg] = max(latest_use_time[inst_list[min_index]->ops[j].reg], t);
            }
        }
        if (inst_list[min_index]->op == OP_STR || inst_list[min_index]->op == OP_BR) { // STR reads from first register
            latest_use_time[inst_list[min_index]->ops[0].reg] = max(latest_use_time[inst_list[min_index]->ops[0].reg], t);
        }
        if (inst_list[min_index]->op == OP_OUT) { // OUT reads from R0
            latest_use_time[0] = max(latest_use_time[0], t);
        }
        inst_list[min_index]->depth = t;
        max_depth = max(max_depth, t);

        min_index++;
    }

    if (inst_list[max_index]->op == OP_BRA) {
        inst_list[max_index]->depth = max_depth;
    }
    
    free(latest_use_time);
    free(completion_time);

    return;
}