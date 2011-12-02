#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "functions.h"

extern int num_errors;
extern int yyerror();
extern int yywrap();
extern int yyparse();
extern int cmdlex();

void c_optimize(void);
void codegen_entry(FILE *fptr);
void codegen_exit(FILE *fptr);
void find_function(void);
void print_inst(FILE*, inst_t, ddg_t *ddg);
void print_list(FILE*, inst_t, ddg_t *ddg);

inst_t instList; /* list of instructions found by parser */
int last_cycle;

int main(int argc, char **argv) {
    arglim = argv + argc;
    targv = argv + 1;

    cmdlex();

    if (outfile == NULL)
        outfile = strdup("out.asm");

    if (infile[0]) {
        c_optimize();
    }

    return 0;
}

void c_optimize() {
    /* file pointer to dump output code */
    FILE *fptr = fopen(outfile, "w");
    block_array cfg;
    ddg_t ddg;
    //int w = 4;
    inst_t *inst_list;

    codegen_entry(fptr);

    yywrap();
    yyparse();

    if (num_errors > 0)
        return;

    if (verbose)
        print_list(stdout, instList, &ddg);

    find_function(); /* remove extra instructions needed for simulation */

    /************************************************************************/
    /************************************************************************/
    /************************************************************************/
    /************************************************************************/
    /*    Call your implementation from here                                */

    /* Find single basic block loops and perform Iterative Modulo Scheduling */

    last_cycle = -1;
    cfg = generate_cfg();
    ddg = generate_ddg();
    calc_depth();
    inst_list = sort_by_depth();
    cycle_schedule(inst_list, &ddg, w);

    /*inst_t list;
    for (list = instList; list; list = list->next) {
        if (list->label) 
                printf("%s:\t", list->label);

        printf("%d\n", list->depth);
    }*/

    sort_by_cycle(&ddg, inst_list);


    if (flag_regalloc) {
        // perform register allocation
        printf("Perform register allocation.\n"); // REMOVE ME
    }

    if (flag_sched) {
        // perform scheduling      
        printf("Perform scheduling.\n"); // REMOVE ME      
    }


    /************************************************************************/
    /************************************************************************/
    /************************************************************************/
    /************************************************************************/

    print_list(fptr, instList, &ddg); /* dump code to output file */

    codegen_exit(fptr);
    fclose(fptr); /* close file */
}

/**************************************************************************
 *   Support for generating code
 */

void codegen_entry(FILE *fptr) {
    fprintf(fptr, "\t.ORIG x2000\n");
    fprintf(fptr, "\tJSR main\n");
    fprintf(fptr, "\tHALT\n");
}

void codegen_exit(FILE *fptr) {
    fprintf(fptr, "\t.END\n");
}

void find_function() {
    /* Need to remove first three instructions */
    inst_t tmp, tmp1, tmp2;
    inst_t oldhead;
    int found = 0;

    if (instList->op == OP_ORIG) {
        tmp = instList->next;
        if (tmp->op == OP_JSR && strcmp(tmp->ops[0].label, "main") == 0) {
            tmp = tmp->next;
            if (tmp->op == OP_HALT) {
                found = 1;
                tmp = tmp->next;
                while (instList != tmp) {
                    oldhead = instList;
                    instList = instList->next;
                    free(oldhead);
                }
            }
        }
    }

    if (!found) {
        printf("Warning: Beginning of input not in the expected format!\n");
    }

    /* Remove last instruction: END */
    tmp = instList;
    tmp1 = tmp;
    tmp2 = tmp1;
    while (tmp != NULL) {
        tmp2 = tmp1;
        tmp1 = tmp;
        tmp = tmp->next;
    }

    if (tmp1->op == OP_END && tmp2 != tmp1) {
        tmp2->next = NULL;
        free(tmp1);
    } else {
        /*printf("Warning: Did not find .END.\n");*/
    }
}

void print_cc(FILE *fptr, int ccode) {
    if (ccode & CC_N)
        fprintf(fptr, "n");
    if (ccode & CC_Z)
        fprintf(fptr, "z");
    if (ccode & CC_P)
        fprintf(fptr, "p");

    fprintf(fptr, " ");
}

void print_op(FILE *fptr, struct operand op) {
    enum op_type t = op.t;
    switch (t) {
        case op_reg:
            fprintf(fptr, "R%d", op.reg);
            break;
        case op_imm:
            fprintf(fptr, "#%d", op.imm);
            break;
        case op_label:
            fprintf(fptr, "%s", op.label);
            break;
    }
}

void print_inst(FILE* fptr, inst_t i, ddg_t *ddg) {
#ifdef debug
    fprintf(fptr, "count = %d\t", i->count);
    fprintf(fptr, "depth = %d\t", i->depth);
#endif   

    int current_cycle = ddg->schedule_time[i->count];
    //printf("current cycle = %d\n", current_cycle);
    //printf("last cycle = %d\n", last_cycle);

    if (current_cycle == last_cycle)
        fprintf(fptr, " . ");
    else if (last_cycle != -1)
        fprintf(fptr, "\n");

    if (i->label) {
        fprintf(fptr, "%s:", i->label);
    }

    if (current_cycle != last_cycle)
        fprintf(fptr, "\t");

    if (i->op == OP_BR) {
        fprintf(fptr, "%s", opnames[i->op]);
        print_cc(fptr, i->ccode);
    } else
        fprintf(fptr, "%s ", opnames[i->op]);

    switch (i->op) {

            /* 3 operands */
        case OP_ADD:
        case OP_AND:
        case OP_ANDL:
        case OP_DIV:
        case OP_LDR:
        case OP_MUL:
        case OP_OR:
        case OP_ORL:
        case OP_STR:
        case OP_SUB:
            print_op(fptr, i->ops[0]);
            fprintf(fptr, ", ");
            print_op(fptr, i->ops[1]);
            fprintf(fptr, ", ");
            print_op(fptr, i->ops[2]);
            break;
            /* 2 operands */
        case OP_BR:
        case OP_SET:
        case OP_ST:
        case OP_STI:
        case OP_LD:
        case OP_LDI:
        case OP_LEA:
        case OP_NOT:
        case OP_NOTL:
            print_op(fptr, i->ops[0]);
            fprintf(fptr, ", ");
            print_op(fptr, i->ops[1]);
            break;

            /* one operand */
        case OP_JSRR:
        case OP_BRA:
        case OP_JMP:
        case OP_JSR:
            print_op(fptr, i->ops[0]);

        default:
            break;
    }
    last_cycle = current_cycle;
    //fprintf(fptr, "\n");
}

void print_list(FILE *fptr, inst_t head, ddg_t *ddg) {
    while (head) {
        print_inst(fptr, head, ddg);
        head = head->next;
    }
}
