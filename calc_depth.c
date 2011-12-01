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

void calc_depth() {

    int MAX_REGS;
    int *latest_use_time, *completion_time;
    inst_t list = instList;
    int t, j;
    
    MAX_REGS = number_of_registers();

    latest_use_time = (int *) malloc(MAX_REGS * sizeof (int));
    completion_time = (int *) malloc(MAX_REGS * sizeof (int));

    while (list) {
        if (list->ops[0].t == op_reg) {
            t = max(1, latest_use_time[list->ops[0].reg] - latency(list) + 1);
        }
        for (j = 1; j <= 2; j++) {
            if (list->ops[j].t == op_reg) {
                t = max(t, completion_time[list->ops[j].reg]);
            }
        }
        if (list->ops[0].t == op_reg) {
            t = max(t, completion_time[list->ops[0].reg] - latency(list));
        }
        if (list->ops[0].t == op_reg) {
            completion_time[list->ops[0].reg] = t + latency(list);
        }
        for (j = 1; j <= 2; j++) {
            if (list->ops[j].t == op_reg) {
                latest_use_time[list->ops[j].reg] = max(latest_use_time[list->ops[j].reg], t);
            }
        }
        list->depth = t;

        list = list->next;
    }
}