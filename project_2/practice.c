#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFSIZE 8192
#define NUMBER_OF_BOOKS 20

int search(char *file_name, char *book_info);
int insert(int file_desc, char *file_name, char *book_info);
int delete(int file_desc, char *file_name, char *book_info);
int replace(int file_desc, char *file_name, char *book_info);

int main(int argc, char *argv[])
{

	int n, comp, fd; 
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
	/*
	for(int i=0; i<NUMBER_OF_BOOKS; i++)
	{	if((bytesWritten = write(fd, books[i], strlen(books[i]))) < strlen(books[i]))
			fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
		else
			;//printf("Data printed to file %d successfully: %s", fileDesc, books[i]);
	}
	*/
	/**********************************************************************************************/
	/** prompt user to enter command and handle input *********************************************/
	bytesWritten = write(STDOUT_FILENO, prompt, strlen(prompt));

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

				if( (search(filename, buffer)!=0) )
					printf("Found in record.\n");
		
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

		write(STDOUT_FILENO, prompt, strlen(prompt));
	}
	/*****************************************************************************************************/
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
			return byte_position;

		else
			byte_position += strlen(book_info);
	}

	byte_position = 0; //record is not found
	return byte_position;
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
int delete(int file_desc, char* file_name, char *book_info)
{
	int byte_position = 0;
	if( (byte_position = search(file_name, book_info)) != 0)
	{
		printf("Delete function here\n");
	}
	
	else
		printf("The record you are attempting to delete does not exist.\n");
}
int replace(int file_desc, char *file_name, char *book_info)
{
	int byte_position = 0;
	if( (byte_position = search(file_name, book_info)) != 0)
	{
		printf("replace function here.\n");
	}
	
	else
		printf("The record you are attempting to replace does not exist.\n");
}
