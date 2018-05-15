/*
On linux system compile like so: gcc -o output_file program.c -lpthread
*/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

/** Macros for File_I/O **/
#define INPUT_BUFF_SIZE 1024
#define FILE_BUFF_SIZE 8192
#define NUMBER_OF_BOOKS 3

/** Macros for client/server **/
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
/** Client/Server functions ***************************************************************/
void *server_routine(void *arg);
void server_request(int operation, int sync, const char *prompt, char *string);
void *client_routine(void *arg);
/******************************************************************************************/
/** function for clearing character buffer ************************************************/
void clear_char_buffer(char buffer[], int max_size)
{
	size_t end_of_string = strlen(buffer) +1; //include null terminating character
	for(int i=0; i< max_size && i < end_of_string; i++)
		buffer[i] = 0;
}
/** File I/O functions *********************************************************************/
int search(char *file_name, char *book_info, int* byte_position);
int insert(int file_desc, char *file_name, char *book_info);
int delete(int file_desc, char *file_name, char *book_info, char buffer_1[], char buffer_2[]);
int replace(int file_desc, char *file_name, char *book_info, char buffer_1[], char buffer_2[]);
/*******************************************************************************************/
char input_buffer[INPUT_BUFF_SIZE];
char file_buffer_1[FILE_BUFF_SIZE];
char file_buffer_2[FILE_BUFF_SIZE];

int n, comp, fd, byte_position;

char *filename = "books.txt";
char *book_info_prompt = "Please enter the title and author info in the following format: BookTitle AuthorName";

/******************************************************************************************/
/** main() body of program ****************************************************************/
int main(int argc, char **argv)
{
	pthread_t thread;

	int count, err;  



	/** Create file to write to ******************************************************/
	mode_t mode = S_IRWXU; //This is equivalent to ‘(S_IRUSR | S_IWUSR | S_IXUSR)

	if((fd = open("books.txt", O_CREAT|O_RDWR|O_TRUNC, mode)) == -1)
		fprintf(stderr, "Error opening/creating file: %s\n", strerror(errno));
	else
		printf("File opened/created successfully.\n");
	/********************************************************************************/
	/** write book titles to file ***************************************************/
	ssize_t bytesWritten, bytesRead;
	const char *books[NUMBER_OF_BOOKS];

	books[0] = "EastOfEden JohnSteinbeck\n";
	books[1] = "FiresOfHeaven RobertJordan\n";
	books[2] = "BloodMeridian CormackMcCarthy\n";
	
	for(int i=0; i < NUMBER_OF_BOOKS; i++)
	{	
		if((bytesWritten = write(fd, books[i], strlen(books[i]))) < strlen(books[i]))
			fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
		else
			printf("Data printed to file %d successfully: %s", fd, books[i]);
	}
	/*******************************************************************************/

	client_threads=CLIENT_THREADS;
	
	/** create 4 threads ***************************************************/
	for(count=0; count<client_threads; count++)
	{	
		printf("Creating client #%d...\n", count);
		err=pthread_create(&thread, NULL, client_routine, (void *)&count);
		//pthread_join(thread, NULL);
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
			printf("%s\n", request->text);
			break;

		case WRITE_REQUEST:
			puts(request->text);
			break;

		case DB_SEARCH:

			printf("You have entered SEARCH, %s\n", book_info_prompt);

			if( (read(STDIN_FILENO, input_buffer, INPUT_BUFF_SIZE) > 0) )
			{

				if( (search(filename, input_buffer, &byte_position)==1) )
					printf("Found in record at byte position: %d.\n", byte_position);

		
				else
					printf("Not found in record.\n");
			}

			else
				printf("Sorry, you did not enter any data to search for in the file.\n");
			break;

		case DB_INSERT:
			printf("You have entered INSERT, %s\n", book_info_prompt);

			if(read(STDIN_FILENO, input_buffer, INPUT_BUFF_SIZE) >0)
				insert(fd, filename, input_buffer);
			
			else
				printf("Sorry, you did not enter any data to insert into the file.\n");
			break;

		case DB_DELETE:
			printf("You have entered DELETE, %s\n", book_info_prompt);

			if(read(STDIN_FILENO, input_buffer, INPUT_BUFF_SIZE) > 0)
				delete(fd, filename, input_buffer, file_buffer_1, file_buffer_2);

			else
				printf("Sorry, you did not enter any data to insert into the file.\n");
			break;

		case DB_REPLACE:
			printf("You have entered REPLACE, %s\n", book_info_prompt);

			if(read(STDIN_FILENO, input_buffer, INPUT_BUFF_SIZE) > 0)
				replace(fd, filename, input_buffer, file_buffer_1, file_buffer_2);
			else
				printf("Sorry, you did not enter any data to insert into the file.\n");
		
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
	printf("In server_request\n");	

	request_t *request; //analogous to node in linked list
	int err;
	
	err=pthread_mutex_lock(&server.mutex);

	if(!server.running)
	{
		/** thread ID and thread attribute **/
		pthread_t thread;
		pthread_attr_t detached_attr;

		err=pthread_attr_init(&detached_attr);
		err=pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);

		server.running=1;

		//printf("server_request creating thread and calling server_routine.\n");
		err=pthread_create(&thread, &detached_attr, server_routine, NULL);

		pthread_attr_destroy(&detached_attr);
	}

	//printf("Before malloc\n");
	/** create request **********************************************/
	request=(request_t *)malloc(sizeof(request_t));
	//printf("After malloc\n");
	request->next=NULL;
	//printf("server_request create request\n");
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
	{
		strncpy(request->prompt, prompt, 32);
		printf("5--------------------------------------------\n");
	}

	else
	{
		request->prompt[0]='\0';
	}
	/***************************************************************/
	/** ONLY THE WRITE REQUESTS GET TO WRITE ***********************/
	if((operation==WRITE_REQUEST) && string!=NULL)
	{
		strncpy(request->text, string, 128);
	}

	else
	{
		request->text[0]='\0';
	}
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
		{
			err=pthread_cond_wait(&request->done, &server.mutex);
		}

		if(operation==READ_REQUEST)
		{
			strcpy(string, request->text);
		}

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

	//sprintf(prompt, "Thread %lu prompt:\n", pthread_self());
	sprintf(prompt, "Client %d prompt:\n", client_number);
	puts(prompt);

	while(1)
	{		
		server_request(READ_REQUEST, 1, prompt, string);

		if(strlen(string)==0)
			break;

		sprintf(formatted, "(%d) %s", client_number, string);
		//printf("formatted: %s\n", formatted);

		//printf("Client routine calling server_request(Write_Request).\n");
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
/** End of client/server functions *********************************************************************/
/*******************************************************************************************************/

/*******************************************************************************************************/
/** File_I/O functions *********************************************************************************/
int search(char *file_name, char *book_info, int *byte_position)
{
	FILE* fp;
	char book_info_comp[256];
	*byte_position = 0;

	fp = fopen(file_name, "r+"); //opens file to update both reading and writing, file must exist
	if(fp==NULL)
	{
		printf("Error opening file: %s\n", strerror(errno));
		return -1;
	}

	while( fgets(book_info_comp, 256, fp) != NULL) //reads entire line from text file
	{ 
		if( strncmp(book_info, book_info_comp, strlen(book_info)) == 0 )
		{
			fclose(fp);
			return 1;
		}

		else
			*byte_position += strlen(book_info_comp);
	}

	fclose(fp);
	return -1;
}
int insert(int file_desc, char *file_name, char *book_info)
{
	int success = 0;
	int byte_position = 0;

	if( search(file_name, book_info, &byte_position) == -1 )
	{
		pwrite(file_desc, book_info, strlen(book_info), lseek(file_desc, 0, SEEK_END));
		success = 1;
	}

	else
		printf("Book is already in file.\n");

	return success;
}   
int delete(int file_desc, char* file_name, char *book_info, char buffer_1[], char buffer_2[])
{
	int byte_position = 0;
	int fd;

	ssize_t bytes_read_1, bytes_read_2, total_length;

	clear_char_buffer(buffer_1, FILE_BUFF_SIZE);
	clear_char_buffer(buffer_2, FILE_BUFF_SIZE);

	if( search(file_name, book_info, &byte_position) == 1) //entry exists in file to delete
	{
		if((bytes_read_1=pread(file_desc, buffer_1, byte_position, 0) )< 0) //read up to record to be replaced
			printf("Error reading from file number %d\n", file_desc);

		if((bytes_read_2=pread(file_desc, buffer_2, FILE_BUFF_SIZE, (strlen(book_info) + byte_position)))<0)
			printf("Error reading from file number %d\n", file_desc);

		else
		{	
			/********************************************************************************/
			mode_t mode = S_IRWXU; //This is equivalent to ‘(S_IRUSR | S_IWUSR | S_IXUSR)

			if((fd = open(file_name, O_CREAT|O_RDWR|O_TRUNC, mode)) == -1)
				fprintf(stderr, "Error opening/creating file: %s\n", strerror(errno));
			/********************************************************************************/

			if(pwrite(fd, buffer_1, strlen(buffer_1), 0) < strlen(buffer_1) )
				printf("Error writing to file %d", file_desc);

			if(pwrite(fd, buffer_2, strlen(buffer_2), strlen(buffer_1)) < strlen(buffer_2) )
				printf("Error writing to file %d", file_desc);
			/********************************************************************************/
			printf("Delete entry at byte_position: %d\n", byte_position);

			return 1;
		}	
	}
	
	else
	{
		printf("The record you are attempting to delete does not exist.\n");
		return -1;
	}
}
int replace(int file_desc, char *file_name, char *book_info, char buffer_1[], char buffer_2[])
{
	int byte_position = 0;
	int fd;
	char replacement_entry[INPUT_BUFF_SIZE];

	clear_char_buffer(buffer_1, FILE_BUFF_SIZE);
	clear_char_buffer(buffer_2, FILE_BUFF_SIZE);

	if( search(file_name, book_info, &byte_position) == 1) //entry exists in file to delete
	{
		printf("Please enter the book title and author, in the appropriate format, that you would like to replace the old entry with.\n");
		if(read(STDIN_FILENO, replacement_entry, INPUT_BUFF_SIZE) < 0)
		{
			printf("Error reading entry to replace with, error: %s\n", strerror(errno));
			return -1;
		}		

		if(pread(file_desc, buffer_1, byte_position, 0)< 0) //read up to record to be replaced
			printf("Error reading from file number %d\n", file_desc);

		if(pread(file_desc, buffer_2, FILE_BUFF_SIZE, strlen(book_info) + byte_position)<0)
			printf("Error reading from file number %d\n", file_desc);

		else
		{
			printf("buffer_1 %s\n", buffer_1);
			printf("replacement text: %s\n", replacement_entry);
			printf("buffer_2 %s\n", buffer_2);
			/********************************************************************************/
			mode_t mode = S_IRWXU; //This is equivalent to ‘(S_IRUSR | S_IWUSR | S_IXUSR)

			if((fd = open(file_name, O_CREAT|O_RDWR|O_TRUNC, mode)) == -1)
				fprintf(stderr, "Error opening/creating file: %s\n", strerror(errno));
			/********************************************************************************/

			if(pwrite(fd, buffer_1, strlen(buffer_1), 0) < strlen(buffer_1) )
				printf("Error writing buffer_1 to file %d", file_desc);

			if(pwrite(fd, buffer_2, strlen(buffer_2), strlen(buffer_1)) < strlen(buffer_2) )
				printf("Error writing buffer_2 to file %d", file_desc);

			if(pwrite(fd, replacement_entry, strlen(replacement_entry), (strlen(buffer_1) + strlen(buffer_2))) < strlen(replacement_entry) )
				printf("Error writing replacement entry to file %d\n", file_desc);
			/*********************************************************************************/

			printf("Replaced entry at byte_position: %d\n", byte_position);
			
			return 1;
		}	
	}
	
	else
	{
		printf("The record you are attempting to replace does not exist.\n");
		return -1;
	}
}
