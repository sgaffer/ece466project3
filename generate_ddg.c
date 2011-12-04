#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern inst_t instList;
extern int count;

ddg_t generate_ddg() {

    int i, j;
    int MAX_REGS;
    inst_t list = instList;
    ddg_t ddg;
    int instr_count = count;
    instr_set *temp;
    inst_t prev = NULL;

    MAX_REGS = number_of_registers();

    ddg.def_inst = (int *) malloc(MAX_REGS * sizeof (int));
    ddg.use_inst = (instr_set *) malloc(MAX_REGS * sizeof (instr_set));
    //int              def_inst[MAX_REGS];
    //instruction_set  use_inst[MAX_REGS];

    ddg.flow_arc = (int**) malloc(instr_count * sizeof (int *));
    ddg.anti_arc = (int**) malloc(instr_count * sizeof (int *));
    ddg.output_arc = (int**) malloc(instr_count * sizeof (int *));

    for (i = 0; i < instr_count; ddg.flow_arc[i++] = (int*) malloc(instr_count * sizeof (int)));
    for (i = 0; i < instr_count; ddg.anti_arc[i++] = (int*) malloc(instr_count * sizeof (int)));
    for (i = 0; i < instr_count; ddg.output_arc[i++] = (int*) malloc(instr_count * sizeof (int)));
    //int flow_arc[n][n];   /* flow dependence edges   */
    //int anti_arc[n][n];   /* anti dependence edges   */
    //int output_arc[n][n]; /* output dependence edges */

    for (i = 0; i < MAX_REGS; ddg.def_inst[i++] = -1); // initialize def_inst
    for (i = 0; i < MAX_REGS; i++) { // initialize use_inst
        ddg.use_inst[i].instr = -1;
        ddg.use_inst[i].next = NULL;
        ddg.use_inst[i].prev = NULL;
    }
    for (i = 0; i < instr_count; i++) { // initialize flow_arc, anti_arc, output_arc to 0
        for (j = 0; j < instr_count; j++) {
            ddg.flow_arc[i][j] = 0;
            ddg.anti_arc[i][j] = 0;
            ddg.output_arc[i][j] = 0;
        }
    }

    while (list) {
        for (j = 1; j <= 2; j++) { // reads from ops[1] & ops[2] for all but STR
            if (list->ops[j].t == op_reg) {
                if (ddg.def_inst[list->ops[j].reg] != -1) {
                    ddg.flow_arc[ddg.def_inst[list->ops[j].reg]][list->count] = 1;
                }
            }
        }
        if (list->op == OP_STR || list->op == OP_BR) { // STR reads from ops[0] too
            if (ddg.def_inst[list->ops[0].reg] != -1) {
                ddg.flow_arc[ddg.def_inst[list->ops[0].reg]][list->count] = 1;
            }
        }
        if (list->op == OP_OUT) { // reads from R0
            if (ddg.def_inst[0] != -1) {
                ddg.flow_arc[ddg.def_inst[0]][list->count] = 1;
            }
        }
        if (prev != NULL) { // reads from previously written register
            if (list->op == OP_RET || list->op == OP_BRA) {
                if (prev->ops[0].t == op_reg) {
                    if (ddg.def_inst[prev->ops[0].reg] != -1) {
                        ddg.flow_arc[ddg.def_inst[prev->ops[0].reg]][list->count] = 1;
                    }
                }
            }
        }
        if (list->ops[0].t == op_reg && list->op != OP_STR && list->op != OP_BR) {
            if (ddg.def_inst[list->ops[0].reg] != -1) {
                ddg.output_arc[ddg.def_inst[list->ops[0].reg]][list->count] = 1;
            }
        }
        if (list->op == OP_IN) { // writes to R0
            if (ddg.def_inst[0] != -1) {
                ddg.output_arc[ddg.def_inst[0]][list->count] = 1;
            }
            if (prev != NULL) { // set fake output dependency on previous instruction
                if (ddg.def_inst[prev->ops[0].reg] != -1) {
                    ddg.output_arc[ddg.def_inst[prev->ops[0].reg]][list->count] = 1;
                }
            }
        }
        if (list->ops[0].t == op_reg && list->op != OP_STR && list->op != OP_BR) {
            for (temp = &ddg.use_inst[list->ops[0].reg]; temp->next != NULL; temp = temp->next) {
                if (temp->instr != -1)
                    ddg.anti_arc[temp->instr][list->count] = 1;
            }
        }
        if (list->op == OP_IN) { // writes to R0
            for (temp = &ddg.use_inst[0]; temp->next != NULL; temp = temp->next) {
                if (temp->instr != -1)
                    ddg.anti_arc[temp->instr][list->count] = 1;
            }
            if (prev != NULL) { // set fake output dependency on previous instruction
                for (temp = &ddg.use_inst[prev->ops[0].reg]; temp->next != NULL; temp = temp->next) {
                    if (temp->instr != -1)
                        ddg.anti_arc[temp->instr][list->count] = 1;
                }
            }
        }
        for (j = 1; j <= 2; j++) {
            if (list->ops[j].t == op_reg) {
                for (temp = &ddg.use_inst[list->ops[j].reg]; temp->next != NULL; temp = temp->next);
                temp->next = (instr_set *) malloc(sizeof (instr_set));
                temp->next->instr = list->ops[j].reg;
                temp->next->next = NULL;
                temp->next->prev = temp;
            }
        }
        if (list->op == OP_STR || list->op == OP_BR) { // STR reads from ops[0] too
            for (temp = &ddg.use_inst[list->ops[0].reg]; temp->next != NULL; temp = temp->next);
            temp->next = (instr_set *) malloc(sizeof (instr_set));
            temp->next->instr = list->ops[0].reg;
            temp->next->next = NULL;
            temp->next->prev = temp;
        }
        if (list->op == OP_OUT) { // reads from R0
            for (temp = &ddg.use_inst[0]; temp->next != NULL; temp = temp->next);
            temp->next = (instr_set *) malloc(sizeof (instr_set));
            temp->next->instr = 0;
            temp->next->next = NULL;
            temp->next->prev = temp;
        }
        if (prev != NULL) { // reads from previously written register
            if (list->op == OP_RET || list->op == OP_BRA) {
                for (temp = &ddg.use_inst[prev->ops[0].reg]; temp->next != NULL; temp = temp->next);
                temp->next = (instr_set *) malloc(sizeof (instr_set));
                temp->next->instr = prev->ops[0].reg;
                temp->next->next = NULL;
                temp->next->prev = temp;
            }
        }
        if (list->ops[0].t == op_reg && list->op != OP_STR && list->op != OP_BR) {
            if (ddg.use_inst[list->ops[0].reg].next != NULL) {
                for (temp = &ddg.use_inst[list->ops[0].reg]; temp->next != NULL; temp = temp->next);
                do {
                    temp = temp->prev;
                    free(temp->next);
                    temp->next = NULL;
                } while (temp != &ddg.use_inst[list->ops[0].reg]);
                temp->instr = -1;
            }
            ddg.def_inst[list->ops[0].reg] = list->count;
        }
        if (list->op == OP_IN) { // writes to R0
            if (ddg.use_inst[0].next != NULL) {
                for (temp = &ddg.use_inst[0]; temp->next != NULL; temp = temp->next);
                do {
                    temp = temp->prev;
                    free(temp->next);
                    temp->next = NULL;
                } while (temp != &ddg.use_inst[0]);
                temp->instr = -1;
            }
            ddg.def_inst[0] = list->count;
            if (prev != NULL) { // set fake output dependency on previous instruction
                if (ddg.use_inst[prev->ops[0].reg].next != NULL) {
                    for (temp = &ddg.use_inst[prev->ops[0].reg]; temp->next != NULL; temp = temp->next);
                    do {
                        temp = temp->prev;
                        free(temp->next);
                        temp->next = NULL;
                    } while (temp != &ddg.use_inst[prev->ops[0].reg]);
                    temp->instr = -1;
                }
                ddg.def_inst[prev->ops[0].reg] = list->count;
            }
        }
        prev = list;
        list = list->next;
    }

    return ddg;
}
