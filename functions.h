/* 
 * File:   functions.h
 * Author: sgaffer
 *
 * Created on November 28, 2011, 10:03 PM
 */

#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

//#define debug true
//#define MULTIOP true

block_array generate_cfg();
ddg_t generate_ddg();
void calc_depth();
int latency(inst_t list);
int max(int first, int second);
int number_of_registers();
int cycle_schedule(inst_t *inst_list, ddg_t *ddg, int slots, int min_index, int max_index, int offset);
void sort_by_depth(inst_t *inst_list);
void sort_by_cycle(ddg_t *ddg, inst_t *inst_list);
void calcLiveness(block_array* cfg);
intNode* calcInterference(live_range* live, int size);
void regAlloc(intNode* intGraph, int size, int k, inst_t instList);

#endif	/* FUNCTIONS_H */

