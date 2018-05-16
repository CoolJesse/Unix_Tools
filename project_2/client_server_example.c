#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h> /* for open() */
#include <sys/stat.h> /* for file access permission bits */
#include <unistd.h> /* for pathconf() */
#include <errno.h> /* for errno */
#include <stdbool.h>
#include <regex.h>
#include <stdint.h>
#define CLIENT_THREADS 4
#define READ_REQUEST 1
#define WRITE_REQUEST 2
#define QUIT_REQUEST 3
#define DB_SEARCH 4
#define DB_INSERT 5
#define DB_DELETE 6
#define DB_REPLACE 7
/* a per-request struct, one per client thread */
typedef struct request_tag
{
	struct request_tag *next; /* pointer to next request */
	int operation; /* one of search,insert,delete,replace requests */
	int synchronous; /* whether request is synchronous */
	int done_flag; /* predicate this request done */
	pthread_cond_t done;/* wait for this thread to finish */
	char prompt[32]; /* prompt server message to client */
	char text[28]; /* read/write text */
} request_t;
/* a server struct, organized as a client request queue */

typedef struct server_tag
{
	request_t *first; /* first request in the queue */
	request_t *last; /* last request in the queue */
	int running; /* predicate this server running */
	pthread_mutex_t mutex; /* lock around server data */
	pthread_cond_t request; /* wait for a request */
} server_t;

server_t server={NULL, NULL, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
int client_threads; /* client has 4 threads */
pthread_mutex_t client_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clients_done=PTHREAD_COND_INITIALIZER;
char * book_del;
char * book_rep;

void *server_routine(void *arg);
void server_request(int operation, int sync, const char *prompt, char *string);
void *client_routine(void *arg);

int main(int argc, char **argv)
{
	
	bool ex = false;
	char * tmp = (char *)malloc(20);
	pthread_t thread;
	int count;
	int err;
	client_threads = 0;
	book_del = (char *) malloc(20);
	book_rep = (char*) malloc(20);
/* 	while (ex == false)
	{
		printf("Please enter a valid command.\nEnter 1 for DB_SEARCH\nEnter 2 for DB_INSERT\nEnter 3 for DB_REPLACE\nEnter 4 for DB_DELETE\n");
		fgets(tmp,20,stdin);
		//printf("%s\n", tmp);
		if(tmp [0]== '1')
		{
		//search
			//printf("=1\n");
			count = 4;
						
			printf("currently in search mode, enter 0 to exit\n");
			printf("Enter the book record that you would like to search for\n");
			while (1)
			{
				fgets(book_del, 20, stdin);
				int f = strcmp(book_del, "0\n");
				if (f==0)				
					break;
				printf("Looking for %s", book_del);
				//book_del[strlen(book_del)-1]='\0';
				client_threads++;
				err=pthread_create(&thread, NULL, client_routine, (void *)(uintptr_t)count);	
			}
			err=pthread_mutex_lock(&client_mutex);
			while (client_threads>0)
				err=pthread_cond_wait(&clients_done, &client_mutex);
			err=pthread_mutex_unlock(&client_mutex);
		
			
		}
		else if(tmp [0]== '2')
		{	//insert
			//printf("=2\n");
			count = 5;
			printf("currently in insert mode\n");
			printf("Enter the book record that you would like to insert\n");
			while(1)
			{
				fgets(book_del, 20, stdin);
				int f = strcmp(book_del, "0\n");
				if (f==0)				
					break;
				printf("Attempting to insert %s", book_del);
				//book_del[strlen(book_del)-1]='\0';
				client_threads++;
				err=pthread_create(&thread, NULL, client_routine, (void *) (uintptr_t)count);
			}
			err=pthread_mutex_lock(&client_mutex);
			while (client_threads>0)
				err=pthread_cond_wait(&clients_done, &client_mutex);
			err=pthread_mutex_unlock(&client_mutex);
		}
		else if(tmp [0]== '3')
		{	//rep
			//printf("=3\n");
			count = 7;
			printf("currently in replace mode\n");
			printf("Enter the book record that you would like to replace\n");
			while (1)
			{
				fgets(book_del, 20, stdin);
				int f = strcmp(book_del, "0\n");
				if (f==0)				
					break;
				printf("Enter the title of the replacement\n");
				fgets(book_rep, 20, stdin);
				//printf("%s\n", book_rep);
				//book_del[strlen(book_del)-1]='\0';
				client_threads++;
				err=pthread_create(&thread, NULL, client_routine, (void *)(uintptr_t) count);
			}
			err=pthread_mutex_lock(&client_mutex);
			while (client_threads>0)
				err=pthread_cond_wait(&clients_done, &client_mutex);
			err=pthread_mutex_unlock(&client_mutex);
			
		}
		else if(tmp [0]== '4')
		{	//del
			//printf("=4\n");
			printf("currently in delete mode\n");
			printf("Enter the book record that you would like to delete\n");
			while (1)
			{
				fgets(book_del, 20, stdin);
				int f = strcmp(book_del, "0\n");
				if (f==0)				
					break;
				printf("Attempting to delete %s", book_del);
				//book_del[strlen(book_del)-1]='\0';
				count = 6;
				client_threads++;
				err=pthread_create(&thread, NULL, client_routine, (void *)(uintptr_t) count);
			}
			err=pthread_mutex_lock(&client_mutex);
			while (client_threads>0)
				err=pthread_cond_wait(&clients_done, &client_mutex);
			err=pthread_mutex_unlock(&client_mutex);
			
		}
		else
		{
			printf("not a valid command\n");
		}
	
	} */

	
	while (ex == false)
	{
		printf("Please enter a valid command.\nEnter 1 for DB_SEARCH\nEnter 2 for DB_INSERT\nEnter 3 for DB_REPLACE\nEnter 4 for DB_DELETE\n");
		fgets(tmp,20,stdin);
		//printf("%s\n", tmp);
		if(tmp [0]== '1')
		{
		//search
			//printf("=1\n");
			count = 4;
			printf("Enter the book record that you would like to search for\n");
			fgets(book_del, 20, stdin);
			printf("Looking for %s", book_del);
			//book_del[strlen(book_del)-1]='\0';
			client_threads++;
			err=pthread_create(&thread, NULL, client_routine, (void*)(uintptr_t) count);
			err=pthread_mutex_lock(&client_mutex);
			while (client_threads>0)
				err=pthread_cond_wait(&clients_done, &client_mutex);
			err=pthread_mutex_unlock(&client_mutex);
		}
		else if(tmp [0]== '2')
		{	//insert
			//printf("=2\n");
			count = 5;
			printf("Enter the book record that you would like to insert\n");
			fgets(book_del, 20, stdin);
			printf("Attempting to insert %s", book_del);
			//book_del[strlen(book_del)-1]='\0';
			client_threads++;
			err=pthread_create(&thread, NULL, client_routine, (void *)(uintptr_t)count);
			err=pthread_mutex_lock(&client_mutex);
			while (client_threads>0)
				err=pthread_cond_wait(&clients_done, &client_mutex);
			err=pthread_mutex_unlock(&client_mutex);
		}
		else if(tmp [0]== '3')
		{	//rep
			//printf("=3\n");
			count = 7;
			printf("Enter the book record that you would like to replace\n");
			fgets(book_del, 20, stdin);
			printf("Enter the title of the replacement\n");
			fgets(book_rep, 20, stdin);
			//printf("%s\n", book_rep);
			//book_del[strlen(book_del)-1]='\0';
			client_threads++;
			err=pthread_create(&thread, NULL, client_routine, (void *)(uintptr_t)count);
			err=pthread_mutex_lock(&client_mutex);
			while (client_threads>0)
				err=pthread_cond_wait(&clients_done, &client_mutex);
			err=pthread_mutex_unlock(&client_mutex);
			
		}
		else if(tmp [0]== '4')
		{	//del
			//printf("=4\n");
			printf("Enter the book record that you would like to delete\n");
			fgets(book_del, 20, stdin);
			printf("Attempting to delete %s", book_del);
			//book_del[strlen(book_del)-1]='\0';
			count = 6;
			client_threads++;
			err=pthread_create(&thread, NULL, client_routine, (void *)(uintptr_t)count);
			err=pthread_mutex_lock(&client_mutex);
			while (client_threads>0)
				err=pthread_cond_wait(&clients_done, &client_mutex);
			err=pthread_mutex_unlock(&client_mutex);
			
		}
		else
		{
			printf("not a valid command\n");
		}
	
	} 
	
	return 0;
	
}

void *server_routine(void *arg)
{
	//printf("In server_routine()...\n");
	static pthread_mutex_t prompt_mutex=
	PTHREAD_MUTEX_INITIALIZER;
	request_t *request;
	int operation;
	int err;
	bool db_search = false;
	bool db_insert = false;
	bool db_replace = false;
	bool db_delete = false;
	while (1)
	{
		/* lock the server mutex, to protect server-modifying
		data */
		err=pthread_mutex_lock(&server.mutex);
		while (server.first==NULL)
		{
			/* wait for request */
			err=pthread_cond_wait(&server.request, &server.mutex);
		}
		/* dequeue 1st request in the queue */
		request=server.first;
		/* advance the head of the queue */
		server.first=request->next;
		if (server.first==NULL)
			/* if queue is empty, reset tail to NULL */
			server.last=NULL;
		err=pthread_mutex_unlock(&server.mutex);
		/* read what the requested op was: read/write/quit */
		operation=request->operation;
		switch (operation)
		{
		case QUIT_REQUEST:
			break;
		case DB_SEARCH:
			//printf("DB_SEARCH\n");
			db_search = true;
			if (db_search == true)
			{
				FILE * fd3;
				char db_buf[4096];
				fd3 = fopen("hw2.txt", "a+b");
				//fd3 = fopen("hw2.txt", "r");
				char * bu;
				bu = malloc(1000);
				int c;
				size_t n;
				fseek(fd3, 0, SEEK_SET);
				int y=0;
				bool match = false;
				int ct = 0;
				//getting file contents
				while(fgets(db_buf, 4096, fd3) != NULL)
				{
					//comparing the line with the requested book
					int f = strcmp(db_buf,book_del);
					ct++;
				
					//if there is a match
					if (f==0)
					{
						printf("book found\n");
						match = true;
					}
					//looking for a new line in the string to increment the book count
			
				}
					
				//the last character in the last record will not be new line terminated but null character terminated,
				//so changing the last character in the requested record to null terminator, then comparing
				book_del[strlen(book_del)-1]='\0';
				int f = strcmp(db_buf,book_del);
					//printf("stcmp %d\n", f);
					if (f==0)
					{
						printf("book found\n");	
						match = true;		
					}
				fclose(fd3);
				if (match == false)
					printf("book not found\n");
				printf("Book count %d\n", ct);
			}
			break;
		case DB_INSERT:
			db_insert = true;
			if (db_insert == true)
			{
				//printf("DB_INSERT\n");
				//checking to see if the recor exists
				
				FILE * fd4 = fopen("hw2.txt", "a+b");				
				fseek(fd4, 0, SEEK_SET);
				int ct = 0; 
				char rep[1000];
				char * tmp= (char *) malloc(100);
				char * ls2[100];
				bool exist = false;
				while( fgets(rep, 1000, fd4) != NULL)
				{
					//increasing the count
					ct++;
					//checking to see if theyre equal
					int f = strcmp(rep, book_del);
					if (f==0)
					{
						printf("requested record already exists, duplicate will be inserted with '_DUP'\n");
						exist = true;
					}
				}
				//rechecking the last record with the null terminator added to the requested book
							
				book_del[strlen(book_del)-1]='\0';
				int f = strcmp(rep, book_del);
				if (f==0)
				{
					printf("requested record already exists, duplicate will be inserted with '_DUP'\n");
					exist = true;
					
				}
				fclose(fd4);
				//adding the record
				if (exist ==false)
				{
					FILE * fd2;
					fd2=fopen("hw2.txt", "a+b");
					int nb = fseek(fd2, 0, SEEK_END);
					fprintf(fd2, "%s", "\n");
					fprintf(fd2, "%s", book_del);
					fclose(fd2);
					ct++;
					printf("Book inserted. Book count is now: %d\n", ct);
				}
				else
				{
					FILE * fd2;
					fd2=fopen("hw2.txt", "a+b");
					int nb = fseek(fd2, 0, SEEK_END);
					fprintf(fd2, "%s", "\n");
					char * ns = (char*) malloc(20);
					strcpy(ns, book_del);
					strcat(ns, "_DUP");
					fprintf(fd2, "%s", ns);
					fclose(fd2);
					ct++;
					printf("book duplicate inserted. Book count is now: %d\n", ct);
				}
			
			}
			break;
		case DB_REPLACE:
			//printf("DB_REPLACE\n");
			db_replace = true;
			if (db_replace == true)
			{
				FILE * fd4 = fopen("hw2.txt", "a+b");				
				fseek(fd4, 0, SEEK_SET);
				int num_rows=0;
				int ch_ct = 0; //character count
				char rep[1000];
				char * tmp= (char *) malloc(100);
				char * ls2[100];
				char cpy[1000];
				int bytr =0;
				int ct=0;
				char t[100];
				//keeping track of whether the book is in the record or not because it may need to be inserted
				bool exist = false;
				//getting the records
				while( fgets(rep, 1000, fd4) != NULL)
				{
					//copying the record in a tmp variable
					strcpy(tmp, rep);
					//checking to see if theyre equal
					int f = strcmp(tmp, book_del);
					if (f==0)
					{
						printf("record found, replacing\n");
						//copying the new record into an array with all the books
						ls2[ct] = (char*) malloc(strlen(book_rep));
						strcpy(ls2[ct],book_rep);
						//counting the books in the file
						ct++;
						//means the book was found and does not have to be inserted
						exist = true;
					}
					else
					{	//other wise copy the regular book
						ls2[ct] = (char*) malloc(strlen(rep));
						strcpy(ls2[ct],rep);
						ct++;
						//printf("%s",ls2[ct]);
					}
				
				}
				//setting the last char in the strings to the null terminator so it can be compared properly
				book_del[strlen(book_del)-1]='\0';
				book_rep[strlen(book_rep)-1]='\0';
				int f = strcmp(tmp, book_del);
				
				if (f==0)
				{
					printf("record found, replacing\n");
					//setting the 2nd to last index, because thats the record that will be copied, because the last index
					//may be a duplicate
					ls2[ct-1] = (char*) malloc(strlen(book_rep));
					strcpy(ls2[ct-1], book_rep);
					//ls[ct] = tmp;
					//printf("%s",ls2[ct-1]);
					exist = true;
				}
				
			
				//clear the old file, rewrite all the records
				FILE * fd=fopen("hw2.txt", "w");
				for (int x=0;x< ct; x++)
				{
					fprintf(fd, "%s",ls2[x]);	
				}
				fclose(fd);
				
							
				//if the book wasnt in the file, add it
				if (exist == false)
				{	
					printf("The Book you tried to replace is not found in records. the Book was inserted instead\n");
					FILE * fd2;
					fd2=fopen("hw2.txt", "a+b");
					fprintf(fd2, "%s", "\n");
					fprintf(fd2, "%s", book_del);
					fclose(fd2);
					ct++;
				}
				printf("Replace process complete, book count is now : %d\n", ct);
				
			}
			db_replace = false;
			break;
		case DB_DELETE:
			//printf("DB_DELETE\n");
			db_delete = true;
			if (db_delete == true)
			{
				FILE * fd4 = fopen("hw2.txt", "a+b");				
				fseek(fd4, 0, SEEK_SET);
				int num_rows=0;
				int ch_ct = 0; //character count
				char rep[1000];
				char * tmp= (char *) malloc(100);
				char * ls2[100];
				char cpy[1000];
				int bytr =0;
				int ct=0;
				char t[100];
				bool exist = false;
				//getting the books
				while( fgets(rep, 1000, fd4) != NULL)
				{//copying into a a tmp string
					strcpy(tmp, rep);
					//compare the tmp variable to the requested book
					
					///if the book is found dont copy it to the array with all the books
					int f = strcmp(tmp, book_del);
					if (f==0)
					{
						//printf("del record found not copying\n");
						exist = true;
					}
					else
					{	//copy as normal if theyre not equal
						ls2[ct] = (char*) malloc(strlen(rep));
						strcpy(ls2[ct],tmp);
						///increase the book count
						ct++;
					}
				
				}
				//setting the null terminating char
				bool last = false;
				book_del[strlen(book_del)-1]='\0';
				int f = strcmp(tmp, book_del);
				//checking if the last record is equal to the requested book with the null terminator
				if (f==0)
				{	//if its equal, do nothing
					//printf("del nt record found not copying\n");
					exist = true;
				}
				else
				{	//otherwise set the last record in the array to the last record from the file
					last = true;
					ls2[ct] = (char*) malloc(strlen(rep));
					strcpy(ls2[ct], rep);
					//printf("%s",ls2[ct]);
				}				
				//since the original while loop does a check with the last record in the file
				//then there is another check with the null terminating character in the requested book, there
				//may be a duplicate in the record, so if last remains false, lower the book count index
				if (last == false)
					ct--;
				//clear the old file and rewrite the array
				FILE * fd=fopen("hw2.txt", "w");
				for (int x=0;x< ct; x++)
				{
					fprintf(fd, "%s",ls2[x]);	
				}				
				fclose(fd);
				//let the user know whether the book was in the records
				if (exist == false)
					printf("error, book not in records\n");
				else
					printf("Delete Successful.\n");
				printf("Book count: %d\n", ct);
			
			}
			
			db_delete = false;
			break;
		default:
			break;
		}
		/* 0=non-synchronous, 1=synchronous */
		if (request->synchronous==1)
		{
			err=pthread_mutex_lock(&server.mutex);
			/* set the done flag to 1, will be the predicate to
			client to synchronize with other */
			request->done_flag=1;
			/* signal that the request is done processing */
			pthread_cond_signal(&request->done);
			err=pthread_mutex_unlock(&server.mutex);
		}
		else
			free(request);
		if (operation==QUIT_REQUEST)
			break; /* break from the while(1) loop */
	}
	return NULL;
}

void *client_routine(void *arg)
{
	//printf("In client_routine()...\n");
	int client_number=(uintptr_t )arg;
	char prompt[32];
	char string[128], formatted[128];
	int err;
	sprintf(prompt, "Client %d>", client_number);
	//puts(prompt);
	while (1)
	{
		if (client_number == 7)
		{
			server_request(DB_REPLACE, 1, prompt, string);
			break;
		}	

		if (client_number == 4)
		{		
			server_request(DB_SEARCH, 1, prompt, string);
			break;
		}		
		
		
		if (client_number == 5)
		{		
			server_request(DB_INSERT, 1, prompt, string);
			break;
		}		
		
		if (client_number == 6)
		{
			server_request(DB_DELETE, 1, prompt, string);
			break;
		}		
		
		break;	
	}
	err=pthread_mutex_lock(&client_mutex);
	client_threads--;
	if (client_threads<=0)
		err=pthread_cond_signal(&clients_done);
	err=pthread_mutex_unlock(&client_mutex);
	//printf("Exiting client_routine()...\n");
	return NULL;
}

void server_request(int operation, int sync, const char *prompt, char *string)
{
	//printf("in server req...\n");
	request_t *request;
	int err;
	/* lock the server data */
	err=pthread_mutex_lock(&server.mutex);
	/* create the server thread */
	if (!server.running)
	{
		pthread_t thread;
		/* server thread is created as a detached thread */
		pthread_attr_t detached_attr;
		err=pthread_attr_init(&detached_attr);
		err=pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);
		server.running=1;
		err=pthread_create(&thread, &detached_attr, server_routine, NULL);
		pthread_attr_destroy(&detached_attr);
	}
	/* create the request */
	request=(request_t *)malloc(sizeof(request_t));
	request->next=NULL; /* this is tail */
	/* these are going to be passed-in arguments */
	request->operation=operation;
	request->synchronous=sync;
	if (sync)
	{
		/* set the done predicate to 0 */
		request->done_flag=0;
		/* initialize the pthread_cond_t variable "done" */
		err=pthread_cond_init(&request->done, NULL);
	}
	/* all requests get a prompt */
	if (prompt!=NULL)
		;//strncpy(request->prompt, prompt, 32);
	else
		;//request->prompt[0]='\0';
	/* only the write requests get to write */
	if (operation==WRITE_REQUEST && string!=NULL)
		;//strncpy(request->text, string, 128);
	else
		;//request->text[0]='\0';
	/* enqueue the request */
	if (server.first==NULL) /* if queue is empty */
	{
		server.first=request; /* head=tail=request */
		server.last=request;
	}
	else /* if queue is non-empty */
	{
		/* make next of current tail point to request */
		(server.last)->next=request;
		/* make tail equal to request */
		server.last=request;
	}
	/* tell server that there is a request made */
	err=pthread_cond_signal(&server.request);
	/* if request was synchronous, wait for a reply */
	if (sync)
	{
		while (!request->done_flag)
		err=pthread_cond_wait(&request->done, &server.mutex);
		//if (operation==READ_REQUEST)
		//strcpy(string, request->text);
		err=pthread_cond_destroy(&request->done);
		free(request);
	}
	err=pthread_mutex_unlock(&server.mutex);
}
