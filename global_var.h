#ifndef GLOBAL_VAR_H
    #define GLOBAL_VAR_H
//***************** Global Var ******************
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond  = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
bool data_read = false;

#endif
