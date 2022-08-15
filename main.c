#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "jansson.h"
 

//#define _GNU_SOURCE

/* The following piece of code is characterized by implementing a problem of reader-writer type
   but it can be generalize to a client-server implementation: 
   the reader threads reads data from the stdin and stores them in a section of memory. This section
   is read later when the writer thread writes to a memory location. The problem needs both 
   threads are blocked; the writer must wait for the memory location is free, not underfoot; 
   and the second thread needs to have something to print. */
 
/* This solution uses a global lock "lock" to protect all access to shared information.
   It uses two condition variables (cond, cond2) so that each thread can safely sleep with a wait operation.
   A thread notifies another when it has completed about our corresponding condition variable.*/

//*************** Defines ************************
#define MAX_DATASET_SIZE 4999
#define SHARED_MEM_SIZE 512
#define THREAD_NUM 20//10 //20
#define NUM_OF_REQ 20

//************** Data structures *******************
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

//***********************************************

//***************** Global Var ******************
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond  = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
bool data_read = false;

//**************************************************
void read_dataset_and_sort_in_file(bool up_down, const char* input_dataset,const char* requests);
void read_dataset_and_dups_in_file(int dup, const char* input_dataset, const char* requests);
void read_dataset_and_mult_in_file(int mult, const char* input_dataset, const char* requests);
int comp_sou(const void *p, const void *q);
int comp_noz(const void *p, const void *q);
void get_DS(shared_DS* input_ds);
void* client(void* input_shds);
void* server(void* snum);
//**************************************************
/* Comparison function. Receives two generic (void) pointers to the items under comparison. */
int comp_sou(const void *p, const void *q) {
    int x = *(const int *)p;
    int y = *(const int *)q;

    if (x < y)
        return -1;  // Return -1 if you want ascending, 1 if you want descending order. 
    else if (x > y)
        return 1;   // Return 1 if you want ascending, -1 if you want descending order.
    return 0;
    return (x > y) - (x < y);
}

int comp_noz(const void *p, const void *q) {
    int x = *(const int *)p;
    int y = *(const int *)q;

    if (x < y)
        return 1;  // Return -1 if you want ascending, 1 if you want descending order. 
    else if (x > y)
        return -1;   // Return 1 if you want ascending, -1 if you want descending order.
    return 0;
    return (x > y) - (x < y);
}

//***************************************************
void get_DS(shared_DS* input_ds){
 char* dataset = input_ds->req_param1;
 char* req_type = input_ds->req_type;
 char* requests = input_ds->requests;

 switch(req_type[0]){
    case 's':
        if(!strcmp(input_ds->req_param2,"top-down")){
             printf("sort top-down func(%s)\n\n",dataset);
             read_dataset_and_sort_in_file(true,dataset,requests); //nozouli
        }
        else if (!strcmp(input_ds->req_param2,"down-top")){
            printf("sort down-top func(%s)\n\n",dataset);
            read_dataset_and_sort_in_file(false,dataset,requests); //soudi
        }
    break;

    case 'g':
         if(!strcmp(req_type,"get_dups")){
            printf("dup_func(%s)\n\n",dataset);
            read_dataset_and_dups_in_file(input_ds->req_param2_dups,dataset,requests);
         }
         else if(!strcmp(req_type,"get_mult")){
            printf("mult_func(%s)\n\n",dataset);
            read_dataset_and_mult_in_file(input_ds->req_param2_mult,dataset,requests);
         }
    break;

    default: printf("Error! Request Type is not correct\n\n");

 }// end of switch

}//end of get_DS

//**************************************//
 void read_dataset_and_sort_in_file(bool up_down, const char* input_dataset,const char* requests)
 {
    FILE *rf, *wf;
    rf = fopen(input_dataset, "r");

    if (rf == NULL){
        printf("Error Reading Dataset File\n");
        exit(EXIT_FAILURE);
    }

    //read file into array
    int arr[MAX_DATASET_SIZE];
    int k =0;
    while(!feof(rf)){
            fscanf(rf, "%3d", &arr[k]);
            k++;
        }
    fclose(rf);
    
    if (up_down)
        qsort(arr, MAX_DATASET_SIZE, sizeof (int),comp_noz);
    else
        qsort(arr, MAX_DATASET_SIZE, sizeof (int),comp_sou);

    wf = fopen(requests,"w");
        if(wf == NULL){
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
        }
    fprintf(wf,"%s\nSorting %s to file...\n",requests,input_dataset);
    for(int i=0; i < MAX_DATASET_SIZE; i++)
        fprintf(wf,"%d\n",arr[i]); 
    
    fclose(wf);
 } 
void read_dataset_and_dups_in_file(int dup, const char* input_dataset, const char* requests)
{
    FILE *rf, *wf;
    rf = fopen(input_dataset, "r");

    if (rf == NULL){
        printf("Error Reading Dataset File\n");
        exit(EXIT_FAILURE);
    }

    //read file into array
    int arr[MAX_DATASET_SIZE];
    int k =0;
    while(!feof(rf)){
            fscanf(rf, "%3d", &arr[k]);
            k++;
        }
    fclose(rf);

	int counter=0;
	for(int i=0 ; i < MAX_DATASET_SIZE ; i++)
		if(arr[i] == dup)
			counter++;
	wf = fopen(requests,"w");
        if(wf == NULL){
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
        }
    fprintf(wf,"%s\n",requests);
	fprintf(wf,"Within file %s, Number of dupplicates of %d is %d\n",input_dataset,dup,counter);

    fclose(wf);

}
void read_dataset_and_mult_in_file(int mul, const char* input_dataset, const char* requests)
{
    FILE *rf, *wf;
    rf = fopen(input_dataset, "r");

    if (rf == NULL){
        printf("Error Reading Dataset File\n");
        exit(EXIT_FAILURE);
    }

    //read file into array
    int arr[MAX_DATASET_SIZE];
    int k =0;
    while(!feof(rf)){
            fscanf(rf, "%3d", &arr[k]);
            k++;
        }
    fclose(rf);

	int counter=0;
	for(int i=0 ; i < MAX_DATASET_SIZE ; i++)
		if(arr[i] % mul == 0)
			counter++;
	wf = fopen(requests,"w");
        if(wf == NULL){
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
        }
    fprintf(wf,"%s\n",requests);
	fprintf(wf,"Whitin file %s, Number of multiplicand of %d is %d\n",input_dataset,mul,counter);

    fclose(wf);

}


//**************** Client/Server Functions **********************
//***************************************************************
void* client(void* input_shds)
{
 while(true)
	{
	pthread_mutex_lock(&lock);
		while(data_read)
			pthread_cond_wait(&cond2,&lock);
  	/* ftok to generate unique key(ftok - convert a pathname and a project identifier to a System V
       IPC key)*/
	key_t key = ftok("sinacomsys",65);
	// shmget returns an identifier in shmid (upon successful, shmget() returns an identifier for the shared memory segment)
	/* the 0666 sets the access permissions (rwx octal format) of the memory segment while IPC_CREAT tells the system
	 to create a new memory segment for the shared memory*/
	int shmid = shmget(key,SHARED_MEM_SIZE,0666|IPC_CREAT); 
	// shmat to attach to shared memory.
	/* function attaches the shared memory segment associated with the shared memory identifier, shmid,
	 to the address space of the calling process.*/
	shared_DS* shds;
    shds = (shared_DS*) malloc(sizeof(shared_DS));
    shds = shmat(shmid,(void*)0,0);
    memcpy(shds,input_shds,sizeof(shared_DS));
    printf("Client %d wrote Req_id %d in memory (%s)\n",shds->client_id,shds->req_id,shds->req_type);
	//detach from shared memory 
	shmdt(shds);
    data_read=true;
	pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
    usleep(100);
 }
}

//*************************************
void* server(void* snum)
{
  while(true)
	{
    pthread_mutex_lock(&lock);
      while(!data_read)
        pthread_cond_wait(&cond,&lock);
	//ftok to generate unique key
	key_t key = ftok("sinacomsys",65);
	// shmget returns an identifier in shmid 
	int shmid = shmget(key,SHARED_MEM_SIZE,0666|IPC_CREAT); 
	//printf("Ser shmid:%d\n",shmid);
	// shmat to attach to shared memory
	shared_DS* shds; //shared data structure
    shds =  shmat(shmid,(void*)0,0);
	//  printf("Server reading from memory Req_id:%d (%s,%s,%s,%s,%d,%d)\n",shds->req_id,shds->requests,shds->req_type, \
    //  shds->req_param1,shds->req_param2,shds->req_param2_dups,shds->req_param2_mult);
	//**** call various get funcs ******//
    get_DS(shds);
    //detach from shared memory 
	shmdt(shds);
	// destroy the shared 
	//IPC_RMID (0 hex) Removes the shared memory segment identifier shmid from the system and destroy the shared memory segment.
	shmctl(shmid,IPC_RMID,NULL); 
    data_read=false;
	pthread_cond_signal(&cond2);
    pthread_mutex_unlock(&lock);
	usleep(100000); // server accepts 10 req/sec
	
    }
}
//******************************************************************

int main()
{
    shared_DS* shds = (shared_DS*) malloc(NUM_OF_REQ * sizeof(shared_DS));
    json_t* input_json;
    json_error_t error;
    input_json = json_load_file("requests.json", 0, &error);
    json_t* jtemp = NULL;
    json_t* jtemp2 = NULL;
    char* comment = NULL;
    char* Header = NULL;
    int Req_Param2_mult=0, Req_Param2_dups =0 ,Req_ID = 0, Client_ID = 0;
    char* Req_Type = NULL;
    char* Req_Param1 = NULL;
    char* Req_Param2 = NULL;
    char temp_pattern[10];
if(input_json)
  for(int i=0 ; i < NUM_OF_REQ ; i++)
  {
    sprintf(temp_pattern,"Request%d",i+1);
    jtemp = json_object_get(input_json, temp_pattern);
    jtemp2 = json_object_get(jtemp, "comment");
    comment = (char*) json_string_value(jtemp2);
    jtemp2 = json_object_get(jtemp, "Req_Type");
    Req_Type = (char*) json_string_value(jtemp2);
    jtemp2 = json_object_get(jtemp, "Req_Param1");
    Req_Param1 = (char*) json_string_value(jtemp2);
if (!strcmp(Req_Type,"get_mult"))
    {
    jtemp2 = json_object_get(jtemp, "Req_Param2");
    Req_Param2_mult = (int) json_integer_value(jtemp2);
    }
else if (!strcmp(Req_Type,"get_dups"))
    {
    jtemp2 = json_object_get(jtemp, "Req_Param2");
    Req_Param2_dups = (int) json_integer_value(jtemp2);
    }
    jtemp2 = json_object_get(jtemp, "Req_Param2");
    Req_Param2 = (char*) json_string_value(jtemp2);

    jtemp2 = json_object_get(json_object_get(jtemp, "Header"), "Req_ID");
    Req_ID = (int) json_integer_value(jtemp2);
    jtemp2 = json_object_get(json_object_get(jtemp, "Header"), "Client_ID");
    Client_ID = (int) json_integer_value(jtemp2);

    strcpy(shds[i].requests,temp_pattern);
    shds[i].req_id=Req_ID;
    shds[i].client_id=Client_ID; 
    shds[i].comment=comment;
    shds[i].req_type=Req_Type;
    shds[i].req_param1=Req_Param1;
    shds[i].req_param2=Req_Param2;
    shds[i].req_param2_dups=Req_Param2_dups;
    shds[i].req_param2_mult=Req_Param2_mult;
  }
  else
    fprintf( stderr, "error: on line %d: %s\n", error.line, error.text);


	pthread_t client_thread[THREAD_NUM], server_thread[THREAD_NUM];

    for(int i=0; i < THREAD_NUM; i++)
    pthread_create(&client_thread[i],NULL,(void*)client,(void*) &shds[i]);

	for(int i=0; i < THREAD_NUM; i++)
	pthread_create(&server_thread[i],NULL,(void*)server,(void*) NULL);
	
	for(int i=0; i < THREAD_NUM; i++)
    pthread_join(client_thread[i],NULL);
	
	for(int i=0; i < THREAD_NUM; i++)
	pthread_join(server_thread[i],NULL);
	
	pthread_mutex_destroy(&lock);
 
return 0;
}
