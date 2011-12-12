#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "functions.h"

extern inst_t instList;
extern int count;

usedef_t *table;
live_range* liveRange;

void calc_Range(int numInstrs, int numRegs, instr_set* liveIn,

        instr_set* liveOut, live_range* live)


 {

    int i, j;

    int found, rangeStart, rangeEnd;

    instr_set* current;



    for (i = 0; i < numRegs; i++) // iterate through all registers and determine ranges
 {

        rangeStart = -1;

        rangeEnd = -1;



        for (j = 3; j < numInstrs; j++) // interate through all instructions
 {

            current = &liveOut[j];
            //printf("Here\n");

            found = 0;



            while (current != NULL)
 {

                if (current->instr == i)
 {

                    rangeStart = j;

                    //printf("Found Live Out Reg %d, Instruction %d\n", i, j);

                    found = 1;

                    break;

                }
                //printf("Here2\n");

                current = current->next;

            }

            if (found == 1)

                break;

        }



        if (found == 0)

            ; //printf("Error Determining Live In for register %d\n", i);



        found = 0;



        for (j = 3; j < numInstrs; j++) // interate through all instructions
 {

            current = &liveIn[j];



            while (current != NULL)
 {

                if (current->instr == i)
 {

                    rangeEnd = j;

                    //printf("Found Live In Reg %d, Instruction %d\n", i, j);

                    found = 1;

                }

                current = current->next;

            }

        }

        //printf("\n");



        if (found == 0)

            ; //printf("Error Determining Live Out for register %d\n", i);



        live[i].go_live = rangeStart;

        live[i].dead = rangeEnd;

    }

}

int firstInstruction() {

    int index;
    inst_t current = instList;

    while (!(current->label))
        current = current->next;

    index = current->count;

    return index;
}

void unionArrays(int *destArray, int *srcArray, int numRegs) {

    int i;

    for (i = 0; i < numRegs; i++) {
        if (srcArray[i] == 1)
            destArray[i] = 1;
    }

    return;
}

int compareArrays(int *firstArray, int *secondArray, int numRegs) {

    int i;

    for (i = 0; i < numRegs; i++) {
        if (secondArray[i] != firstArray[i])
            return 1;
    }
    return 0;
}

void copyArray(int *firstArray, int *secondArray, int numRegs) {

    int i;

    for (i = 0; i < numRegs; i++) {
        secondArray[i] = firstArray[i];
    }

    return;
}

void genUseDef() {
    inst_t current = instList;

    while (current != NULL) {
        if (current->op == OP_BR) {
            if (current->ops[0].t == op_reg)
                table[current->count].use0 = current->ops[0].reg;
        } else if (current->op == OP_STR) {
            if (current->ops[0].t == op_reg)
                table[current->count].use0 = current->ops[0].reg;
            if (current->ops[1].t == op_reg)
                table[current->count].use1 = current->ops[1].reg;
        } else if (current->op == OP_IN) {
            table[current->count].def = 0;
        } else if (current->op == OP_OUT) {
            table[current->count].use0 = 0;
        } else if (current->op == OP_SET) {
            table[current->count].def = current->ops[0].reg;
        } else if (current->op != OP_BRA && current->op != OP_RET) {
            if (current->ops[0].t == op_reg)
                table[current->count].def = current->ops[0].reg;
            if (current->ops[1].t == op_reg)
                table[current->count].use0 = current->ops[1].reg;
            if (current->ops[2].t == op_reg)
                table[current->count].use1 = current->ops[2].reg;
        }

        current = current->next;
    }
}

void successors(block_array *cfg) {

    block *node = cfg->label_list[0];
    int curr_index = 0;
    int i;

    while (node != NULL) {

        if (node->instruction->label) {
            for (i = 0; i < cfg->num_of_labels; i++) {
                if (strcmp(cfg->label_list[i]->instruction->label, node->instruction->label) == 0) {
                    curr_index = i;
                    break;
                }
            }
        }

        if (node->instruction->op == OP_BR) {
            table[node->instruction->count].succ0 = node->left->instruction->count;
            table[node->instruction->count].succ1 = node->right->instruction->count;
            node = node->right;
        } else if (node->instruction->op == OP_RET) {
            table[node->instruction->count].succ0 = -1;
            table[node->instruction->count].succ1 = -1;
            break;
        } else if (node->instruction->op == OP_BRA) {
            table[node->instruction->count].succ0 = node->left->instruction->count;
            table[node->instruction->count].succ1 = -1;
            node = cfg->label_list[curr_index + 1];
        } else {
            table[node->instruction->count].succ0 = node->left->instruction->count;
            table[node->instruction->count].succ1 = -1;
            node = node->left;
        }
    }

    return;
}

void liveness() {
    int i, j;
    int changed = 1;
    int numRegs = number_of_registers();

    int items;

    instr_set* current;

    int **liveInArray, **liveOutArray;
    int *temp;

    instr_set* liveIn;
    instr_set* liveOut;

    liveRange = (live_range*) malloc(count * sizeof (live_range));

    liveIn = (instr_set*) malloc(count * sizeof (instr_set));
    liveOut = (instr_set*) malloc(count * sizeof (instr_set));

    liveInArray = (int**) malloc(count * sizeof (int*));
    liveOutArray = (int**) malloc(count * sizeof (int*));
    temp = (int*) calloc(numRegs, sizeof (int));

    for (i = 0; i < count; i++) {
        liveInArray[i] = (int*) calloc(numRegs, sizeof (int));
        liveOutArray[i] = (int*) calloc(numRegs, sizeof (int));
    }


    while (changed == 1) // iterate while no changes made
    {
        changed = 0;

        // calc live ins
        for (i = 3; i < count; i++) // iterate through all instructions
        {
            copyArray(liveOutArray[i], temp, numRegs); // make temp array

            temp[table[i].def] = 0;

            if (table[i].use0 != -1)
                temp[table[i].use0] = 1;
            if (table[i].use1 != -1)
                temp[table[i].use1] = 1;

            changed = compareArrays(liveOutArray[i], temp, numRegs);

            copyArray(temp, liveInArray[i], numRegs);
        }

        // calc live outs
        for (i = 3; i < count; i++) // iterate through all instructions
        {
            for (j = 0; j < numRegs; j++) // clear array
                temp[j] = 0;

            // check successor liveness
            if (table[i].succ0 != -1)
                unionArrays(temp, liveInArray[table[i].succ0], numRegs);
            if (table[i].succ1 != -1)
                unionArrays(temp, liveInArray[table[i].succ1], numRegs);

            // check for change
            if (compareArrays(liveOutArray[i], temp, numRegs) == 1)
                changed = 1;

            // copy into full array
            copyArray(temp, liveOutArray[i], numRegs);
        }
    }

    // copy simple array into data structure
    for (i = 0; i < count; i++) {
        current = &liveIn[i];
        items = 0;
        for (j = 0; j < numRegs; j++) {
            if (liveInArray[i][j] == 1) {
                if (items == 0) {
                    current->instr = j;
                    items++;
                } else {
                    current->next = (instr_set*) malloc(sizeof (instr_set));
                    current = current->next;
                    current->instr = j;
                    current->next = NULL;
                    items++;
                }
            }
        }

        if (items == 0) {
            liveIn[i].instr = -1;
            liveIn[i].next = NULL;
        }

        current = &liveOut[i];
        items = 0;
        for (j = 0; j < numRegs; j++) {
            if (liveOutArray[i][j] == 1) {
                if (items == 0) {
                    current->instr = j;
                    items++;
                } else {
                    current->next = (instr_set*) malloc(sizeof (instr_set));
                    current = current->next;
                    current->instr = j;
                    current->next = NULL;
                    items++;
                }
            }
        }

        if (items == 0) {
            liveOut[i].instr = -1;
            liveOut[i].next = NULL;
        }
    }

    calc_Range(count, numRegs, liveIn, liveOut, liveRange);

    // debug printouts
    /*for(i=0;i<numRegs;i++)
    {
            printf("R%d: %02d -> %02d\n", i, liveRange[i].go_live, liveRange[i].dead);
    }*/
}

void calcLiveness(block_array* cfg) {
    int i;

    table = (usedef_t*) malloc(count * sizeof (usedef_t));

    for (i = 0; i < count; i++) {
        table[i].use0 = -1;
        table[i].use1 = -1;
        table[i].def = -1;
        table[i].succ0 = -1;
        table[i].succ1 = -1;
    }

    genUseDef();
    successors(cfg);

    liveness();

    //for(i=3;i<count;i++);
    /*{
            printf("%03d: use0: %02d use1: %02d def: %02d succ0: %02d succ1: %02d\n",
                                    i, table[i].use0, table[i].use1, table[i].def, table[i].succ0, table[i].succ1);
    }*/
}
