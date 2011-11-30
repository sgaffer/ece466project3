/* 
 * File:   functions.h
 * Author: sgaffer
 *
 * Created on November 28, 2011, 10:03 PM
 */

#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

block_array generate_cfg();
ddg_t generate_ddg();
void calc_depth();
int latency(inst_t list);
int max(int first, int second);
int number_of_registers();

#endif	/* FUNCTIONS_H */

