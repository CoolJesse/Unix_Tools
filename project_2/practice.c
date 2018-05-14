#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFSIZE 8192
#define NUMBER_OF_BOOKS 3

void clear_char_buffer(char buffer[], int max_size)
{
	size_t end_of_string = strlen(buffer) +1; //include null terminating character
	for(int i=0; i< max_size && i < end_of_string; i++)
		buffer[i] = 0;
}

int search(char *file_name, char *book_info, int* byte_position);
int insert(int file_desc, char *file_name, char *book_info);
int delete(int file_desc, char *file_name, char *book_info);
int replace(int file_desc, char *file_name, char *book_info);

int main(int argc, char *argv[])
{

	int n, comp, fd, byte_position; 
	char buffer[BUFFSIZE];

	char *filename = "books.txt"; 
	char *prompt = "Please enter a command: SEARCH, INSERT, DELETE, REPLACE, or QUIT> ";
	char *book_info_prompt = "Please enter the title and author info in the following format: BookTitle AuthorName";	

	/** Create file to write to *******************************************************************/
	mode_t mode = S_IRWXU; //This is equivalent to ‘(S_IRUSR | S_IWUSR | S_IXUSR)

	if((fd = open("books.txt", O_CREAT|O_RDWR|O_TRUNC, mode)) == -1)
		fprintf(stderr, "Error opening/creating file: %s\n", strerror(errno));
	else
		printf("File opened/created successfully.\n");
	/**********************************************************************************************/
	/** write book titles to file *****************************************************************/
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
	
	/**********************************************************************************************/
	/** prompt user to enter command and handle input *********************************************/
	bytesWritten = write(STDOUT_FILENO, prompt, strlen(prompt));
	
	clear_char_buffer(buffer, BUFFSIZE);
	while( (n=read(STDIN_FILENO, buffer, BUFFSIZE)) > 0)
	{
		if(write(STDOUT_FILENO, buffer, n) != n)
			printf("WRITE ERROR\n");

		/** search command ***********************************/
		if( strncmp(buffer, "SEARCH", 6) == 0)
		{
			printf("You have entered SEARCH, %s\n", book_info_prompt);

			if( (read(STDIN_FILENO, buffer, BUFFSIZE) > 0) )
			{

				if( (search(filename, buffer, &byte_position)==1) )
					printf("Found in record at byte position: %d.\n", byte_position);

		
				else
					printf("Not found in record.\n");
			}

			else
				printf("Sorry, you did not enter any data to search for in the file.\n");
		}
		
		/** insert command ***********************************/
		else if( strncmp(buffer, "INSERT", 6) == 0)
		{
			printf("You have entered INSERT, %s\n", book_info_prompt);

			if(read(STDIN_FILENO, buffer, BUFFSIZE) >0)
				insert(fd, filename, buffer);
			
			else
				printf("Sorry, you did not enter any data to insert into the file.\n");
		}

		/** delete command **********************************/
		else if( (comp = strncmp(buffer, "DELETE", 6)) == 0)
		{
			printf("You have entered DELETE, %s\n", book_info_prompt);

			if(read(STDIN_FILENO, buffer, BUFFSIZE) > 0)
				delete(fd, filename, buffer);

			else
				printf("Sorry, you did not enter any data to insert into the file.\n");				
		}
		
		/** replace command **********************************/
		else if( (comp = strncmp(buffer, "REPLACE", 7)) == 0)
		{
			printf("You have entered REPLACE, %s\n", book_info_prompt);

			if(read(STDIN_FILENO, buffer, BUFFSIZE) > 0)
				replace(fd, filename, buffer);
			else
				printf("Sorry, you did not enter any data to insert into the file.\n");
		}

		/** quit command *************************************/
		else if( (comp = strncmp(buffer, "QUIT", 4)) == 0)
		{
			printf("You have entered QUIT, goodbye!\n");
			break;
		}

		/** unrecognized command *****************************/
		else
			printf("Sorry, you have entered an invalid command\n");

		clear_char_buffer(buffer, BUFFSIZE);
		write(STDOUT_FILENO, prompt, strlen(prompt));
	}
	/*****************************************************************************************************/
	if(n<0)
		printf("READ ERROR");

	return 0;
}
int search(char *file_name, char *book_info, int *byte_position)
{
	FILE* fp;
	
	char book_info_comp[256];

	//int byte_position = 0;
	*byte_position = 0;

	fp = fopen(file_name, "r+"); //opens file to update both reading and writing, file must exist
	if(fp==NULL)
	{
		printf("Error opening file: %s\n", strerror(errno));
		//return byte_position;
		return -1;
	}

	//printf("Database contents:\n");

	while( fgets(book_info_comp, 256, fp) != NULL) //reads entire line from text file
	{ 
		//printf("Searching for: %s, Found: %s at byte position: %d\n", book_info, book_info_comp, *byte_position);

		if( strncmp(book_info, book_info_comp, strlen(book_info)) == 0 )
		{
			fclose(fp);
			return 1;
		}

		else
			*byte_position += strlen(book_info_comp);
	}

	//byte_position = 0; //record is not found
	fclose(fp);
	//return byte_position;
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
int delete(int file_desc, char* file_name, char *book_info)
{
	int byte_position = 0;
	if( search(file_name, book_info, &byte_position) == 1)
	{
		printf("Delete entry at byte_position: %d\n", byte_position);
	}
	
	else
		printf("The record you are attempting to delete does not exist.\n");
}
int replace(int file_desc, char *file_name, char *book_info)
{
	int byte_position = 0;
	if( search(file_name, book_info, &byte_position) == 1)
	{
		printf("replace entry at byte position: %d\n", byte_position);
	}
	
	else
		printf("The record you are attempting to replace does not exist.\n");
}
