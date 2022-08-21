#include "macros.h"

#ifndef TYPES_H
    #define TYPES_H

typedef struct shared_DS {
	char requests[10]; //string name of req
	char* comment; // request header comments
	char* req_type;// type of req (sort, dup, mul)
    int req_param2_mult; // multiplicant coeff
	int req_param2_dups; //duplicant coeff
	int req_id; //request id
	int client_id; //client id
	char* req_param1; //dataset
	char* req_param2; // if mul or dup its null else its top sort or down sort
	
}shared_DS;

#endif