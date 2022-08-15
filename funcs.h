#include "macros.h"
#include "types.h"

#ifndef FUNCTIONS_H
    #define FUNCTIONS_H

void read_dataset_and_sort_in_file(bool up_down, const char* input_dataset,const char* requests);
void read_dataset_and_dups_in_file(int dup, const char* input_dataset, const char* requests);
void read_dataset_and_mult_in_file(int mult, const char* input_dataset, const char* requests);
int comp_sou(const void *p, const void *q);
int comp_noz(const void *p, const void *q);
void get_DS(shared_DS* input_ds);
void* client(void* input_shds);
void* server(void* snum);
	
#endif