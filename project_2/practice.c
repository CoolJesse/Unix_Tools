#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFSIZE 8192

int search(char *file_name, char *book_info);
int insert(int file_desc, char *file_name, char *book_info);
int delete(char* file_name, char *book_info);
int replace(int file_desc, char *book_author, char *book_title);

int main(int argc, char *argv[])
{

	int n, comp, fd; 
	char buffer[BUFFSIZE];

	char *filename = "books.txt"; 
	char *prompt = "Please enter a command: SEARCH, INSERT, DELETE, REPLACE, or QUIT> ";
	char *book_info_prompt = "Please enter the title and author info in the following format: BookTitle AuthorName";	

	/*********************************** Create file to write to **********************************/
	mode_t mode = S_IRWXU; //This is equivalent to ‘(S_IRUSR | S_IWUSR | S_IXUSR)
	if((fd = open("books.txt", O_CREAT|O_RDWR|O_TRUNC, mode)) == -1)
		fprintf(stderr, "Error opening/creating file: %s\n", strerror(errno));
	else
		printf("File opened/created successfully.\n");
	/**********************************************************************************************/

	write(STDOUT_FILENO, prompt, strlen(prompt));

	while( (n=read(STDIN_FILENO, buffer, BUFFSIZE)) > 0)
	{
		if(write(STDOUT_FILENO, buffer, n) != n)
			printf("WRITE ERROR\n");

		/** search command ***********************************/
		if( strncmp(buffer, "SEARCH", 6) == 0)
		{
			printf("You have entered SEARCH\n");
			printf("%s", book_info_prompt);

			if( (read(STDIN_FILENO, buffer, BUFFSIZE) > 0) )
			{

				if( (search(filename, buffer)!=0) )
				{
					printf("Found %s in record.\n", buffer);
				}

				else
				{
					printf("%s not found in record.\n", buffer);
				}
			}

			else
			{
				printf("Sorry, you did not enter any data to search for in the file.\n");
			}
		}
		
		/** insert command ***********************************/
		else if( strncmp(buffer, "INSERT", 6) == 0)
		{
			printf("You have entered INSERT\n");
			printf("%s", book_info_prompt);

			if(read(STDIN_FILENO, buffer, BUFFSIZE) >0)
				insert(fd, filename, buffer);
			
			else
				printf("Sorry, you did not enter any data to insert into the file.\n");
		}

		/** delete command **********************************/
		else if( (comp = strncmp(buffer, "DELETE", 6)) == 0)
		{
			printf("You have entered DELETE\n");
			printf("%s", book_info_prompt);

			read(STDIN_FILENO, buffer, BUFFSIZE);
		}
		
		/** replace command **********************************/
		else if( (comp = strncmp(buffer, "REPLACE", 7)) == 0)
		{
			printf("You have entered REPLACE\n");
			printf("%s", book_info_prompt);

			read(STDIN_FILENO, buffer, BUFFSIZE);
		}

		/** quit command *************************************/
		else if( (comp = strncmp(buffer, "QUIT", 4)) == 0)
		{
			printf("You have entered QUIT\n");
			break;
		}

		/** unrecognized command *****************************/
		else
			printf("Sorry, you have entered an invalid command\n");

		write(STDOUT_FILENO, prompt, strlen(prompt));
	}

	if(n<0)
		printf("READ ERROR");

	return 0;
}
int search(char *file_name, char *book_info)
{
	FILE* fp;
	
	char book_info_comp[256];

	int found = 0; //not yet found
	int byte_position = 0;

	fp = fopen(file_name, "r+"); //opens file to update both reading and writing, file must exist

	while( fgets(book_info_comp, 256, fp) != NULL) //reads entire line from text file
	{ 
		//fscanf(fp, "%s", book_author_comp); //parses buffer into seperate author and title
		//fscanf(fp, "%s", book_title_comp);

		if( strncmp(book_info, book_info_comp, strlen(book_info)) == 0 )
			return byte_count;

		else
			byte_count += strlen(book_info);
	}

	byte_count = 0; //record is not found
	return byte_count
}
int insert(int file_desc, char *file_name, char *book_info)
{
	int success = 0;

	if( search(file_name, book_info) == 0 )
	{
		pwrite(file_desc, book_info, strlen(book_info), lseek(file_desc, 0, SEEK_END));
		success = 1;
	}

	else
		printf("Book is already in file.\n");

	return success;
}   
int delete(char* file_name, char *book_info)
{
	int byte_position = 0;
	if( (byte_position = search(file_name, book_info)) != 0)
	{
		printf("Delete function here\n");
	}
	
	else
		printf("The record you are attempting to delete does not exist.\n");
}
int replace(int file_desc, char *book_author, char *book_title)
{
}
