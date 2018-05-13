/*
On linux system compile like so: gcc -o output_file program.c -lpthread
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CLIENT_THREADS 4

#define READ_REQUEST 1
#define WRITE_REQUEST 2
#define QUIT_REQUEST 3

#define DB_SEARCH 4
#define DB_INSERT 5
#define DB_DELETE 6
#define DB_REPLACE 7

/******************************************************************************************/
typedef struct request_tag
{
	struct request_tag *next; // pointer to next request
	int operation;            // one of the read/write/quit requests
	int synchronous;          // whether request is synchronous
	int done_flag;            // predicate this request done
	pthread_cond_t done;      // wait for this thread to finish
	char prompt[32];          // prompt server message to client
	char text[28];            // read/write text
}request_t;
/******************************************************************************************/
typedef struct server_tag
{
	request_t *first;       // first request in the queue
	request_t *last;        // last request in the queue
	int running;            // predicate this server running
	pthread_mutex_t mutex;  // lock around server data
	pthread_cond_t request; // wait for a request
}server_t;
/******************************************************************************************/
/** declare and initialize server_t object ************************************************/
server_t server={NULL, NULL, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

/** declare client variables **************************************************************/
int client_threads;
pthread_mutex_t client_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clients_done=PTHREAD_COND_INITIALIZER;

/** Declare functions *********************************************************************/
void *server_routine(void *arg);
void server_request(int operation, int sync, const char *prompt, char *string);
void *client_routine(void *arg);

/** main() body of program ****************************************************************/
int main(int argc, char **argv)
{
	pthread_t thread;
	int count;
	int err;

	client_threads=CLIENT_THREADS;
	
	/** create 4 threads ***************************************************/
	for(count=0; count<client_threads; count++)
	{	
		printf("Creating client #%d...\n", count);
		err=pthread_create(&thread, NULL, client_routine, (void *)&count);
	}
	printf("\n");
	/** lock client mutex **************************************************/
	err=pthread_mutex_lock(&client_mutex);

	while(client_threads > 0)
	{
		/** wait for cond clients_done to change ******************/		
		err=pthread_cond_wait(&clients_done, &client_mutex);
	}
	
	/** unlock client mutex ************************************************/
	err=pthread_mutex_unlock(&client_mutex);

	printf("All clients done.\n");

	/** Issue request to stop server and return to prompt ******************/
	server_request(QUIT_REQUEST, 1, NULL, NULL);

	return 0;
}
/** server_routine() function *************************************************************/
void *server_routine(void *arg)
{
	printf("In server_routine()...\n");

	static pthread_mutex_t prompt_mutex=PTHREAD_MUTEX_INITIALIZER;
	request_t *request;
	int operation;
	int err;

	while(1)
	{
		/** lock the server mutex, to protect server-modifying data **/
		err=pthread_mutex_lock(&server.mutex);

		while(server.first==NULL)
		{
			err=pthread_cond_wait(&server.request, &server.mutex);
		}

		/** dequeue 1st request in the queue **/
		request=server.first;
		/** advance the head of the queue **/
		server.first=request->next;

		if(server.first==NULL)
			server.last=NULL;

		err=pthread_mutex_unlock(&server.mutex);

		/** read what the requested op was: read, write, quit **/
		operation=request->operation;

		switch(operation)
		{
		case QUIT_REQUEST:
			break;

		case READ_REQUEST:
			if(strlen(request->prompt)>0)
				printf("--->%s", request->prompt);

			/** strip newline character **/
			if(fgets(request->text, 128, stdin)==0)
				request->text[0]='\0';

			request->text[strlen(request->text)-1]='\0';
			break;

		case WRITE_REQUEST:
			puts(request->text);
			break;

		case DB_SEARCH:
			break;

		case DB_INSERT:
			break;

		case DB_DELETE:
			break;

		case DB_REPLACE:
			break;
		
		default:
			break;
		}
		
		/** 0=non synchronous, 1=synchronous **/
		if(request->synchronous==1)
		{
			err=pthread_mutex_lock(&server.mutex);

			request->done_flag=1;

			/** signal that the request is done processing **/
			pthread_cond_signal(&request->done);

			err=pthread_mutex_unlock(&server.mutex);
		}

		else
			free(request);

		if(operation==QUIT_REQUEST)
			break;


	}
	
	return NULL;
}
/******************************************************************************************/
/** server_request() function *************************************************************/
void server_request(int operation, int sync, const char *prompt, char *string)
{
	request_t *request; //analogous to node in linked list
	int err;

	err=pthread_mutex_lock(&server.mutex);

	if(!server.running)
	{
		pthread_t thread;

		pthread_attr_t detached_attr;

		err=pthread_attr_init(&detached_attr);
		err=pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);

		server.running=1;

		err=pthread_create(&thread, &detached_attr, server_routine, NULL);

		pthread_attr_destroy(&detached_attr);
	}

	/** create request **********************************************/
	request=(request_t *)malloc(sizeof(request_t));
	request->next=NULL;

	/** call passed in arguements ***********************************/
	request->operation = operation;
	request->synchronous = sync;

	if(sync)
	{
		/** set done predicate to 0 ***********************/
		request->done_flag=0;

		/** initilize the pthread cond_t variable "done" **/
		err=pthread_cond_init(&request->done, NULL);
	}

	/** ALL REQUESTS GET A PROMPT ***********************************/
	if(prompt!=NULL)
		strncpy(request->prompt, prompt, 32);

	else
		request->prompt[0]='\0';
	/***************************************************************/
	/** ONLY THE WRITE REQUESTS GET TO WRITE ***********************/
	if((operation==WRITE_REQUEST) && string!=NULL)
		strncpy(request->text, string, 128);

	else
		request->text[0]='\0';
	/***************************************************************/
	/** enqueue the request ****************************************/
	if(server.first==NULL) /* if the queue is empty */
	{
		server.first=request;
		server.last=request;
	}

	else /* if the queue is not empty*/
	{
		(server.last)->next=request;
		server.last=request;
	}
	/***************************************************************/
	/** tell server that there is a request made *******************/
	err=pthread_cond_signal(&server.request);

	/***************************************************************/
	/** if the request was synchronous, wait for a reply ***********/
	if(sync)
	{
		while(!request->done_flag)
			err=pthread_cond_wait(&request->done, &server.mutex);

		if(operation==READ_REQUEST)
			strcpy(string, request->text);

		err=pthread_cond_destroy(&request->done);
		free(request);
	}

	err=pthread_mutex_unlock(&server.mutex);
}
/******************************************************************************************/
/** Client_routine() function *************************************************************/
void *client_routine(void *arg)
{
	printf("In client_routine()...\n");

	int* temp = (int*)arg;
	int client_number= *temp;

	char prompt[32];
	char string[128];
	char formatted[128];
	int err;

	sprintf(prompt, "Client %d> ", client_number);
	puts(prompt);

	while(1)
	{
		server_request(READ_REQUEST, 1, prompt, string);

		if(strlen(string)==0)
			break;

		sprintf(formatted, "(%d) %s", client_number, string);

		server_request(WRITE_REQUEST, 0, NULL, formatted);
	}

	err=pthread_mutex_lock(&client_mutex);

	client_threads--;

	if(client_threads<=0)
		err=pthread_cond_signal(&clients_done);

	err=pthread_mutex_unlock(&client_mutex);
	printf("Exiting client routine()...\n");

	return NULL;

}
