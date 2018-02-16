#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <inttypes.h>

#define NUMBER_OF_BOOKS 3

int getBytePosition(int row, int filedes);
void insertBook(int fd, int byte, char *BookTitle, char *authorName);

int main(int argc, char *argv[])
{
	/**************************** Get the current working directory *****************************/
	int fileDesc;
	char *fileName = "/books.txt";
	char path_name[1024] = {0};
		
	if(getcwd(path_name, sizeof(path_name)) != NULL)
		printf("Current working directory: %s\n", path_name);
	else
		perror("getcwd() error");

	/********************************************************************************************/

	/************* appends the file name of the file to write to the path ***********************/
	int i = 0;
	for(; path_name[i] != 0; i++)
			; //empty for loop to pass through character array

	for(int j = 0;j < strlen(fileName); j++, i++)
		path_name[i] = fileName[j];

	path_name[i] = '\n';				

	printf("The directory to write to is: %s", path_name);

	/*********************************** Create file to write to **********************************/
	mode_t mode = S_IRWXU; //This is equivalent to ‘(S_IRUSR | S_IWUSR | S_IXUSR)

	if((fileDesc = open(path_name, O_CREAT|O_RDWR|O_TRUNC, mode)) == -1)
		fprintf(stderr, "Error opening/creating file: %s\n", strerror(errno));
	else
		printf("File opened/created successfully.\n");
	/*********************************************************************************************/

	/************************************* Write to File *****************************************/
	//int row;
	ssize_t bytesWritten, bytesRead;
	const char *books[NUMBER_OF_BOOKS];

	books[0] = "EastOfEden JohnSteinbeck\n";
	books[1] = "FiresOfHeaven RobertJordan\n";
	books[2] = "BloodMeridian CormackMcCarthy\n";

	for(int i=0; i<NUMBER_OF_BOOKS; i++)
	{	if((bytesWritten = write(fileDesc, books[i], strlen(books[i]))) < strlen(books[i]))
			fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
		else
			;//printf("Data printed to file %d successfully: %s", fileDesc, books[i]);
	}
	/*********************************************************************************************/

	/************ Reset cursor to beggining of file by calling lseek() function ******************/
	off_t current_position;
	current_position = lseek(fileDesc, 0,SEEK_SET);
	/*********************************************************************************************/

	/******************************** Read contents of file **************************************/
	char readData[4028] = {0};

	if(bytesRead = read(fileDesc, readData, sizeof(readData)) == -1)
		printf("Error reading from file %s\n", strerror(errno));
	else
		printf("The contents of the book.txt file are: \n%s\n", readData);
	/*********************************************************************************************/

	/************************************** Modify file ******************************************/
	
	char answer = 'n';
	char bookTitle[100];
	char bookAuthor[100];
	int row;

	printf("Would you like to replace or insert a book: Y, N?\n");
	scanf("%c", &answer);
	
	while(answer != 'y' && answer!= 'Y' && answer !='n' && answer !='N')
	{	printf("Sorry, you entered an invalid response, please try again\n");
		printf("The contents of the book.txt file are: %s", readData);
		printf("Would you like to replace or insert a book: Y, N?\n");

		scanf("%c", &answer);
	}

	if(answer == 'Y' || answer == 'y')
	{	printf("Which book title would you like to add? Please do not include spaces and do not exceed 100 characters.\n");
		scanf("%s", bookTitle);
		
		printf("What is the name of the author? Please do not include spaces and do not exceed 100 characters.\n");
		scanf("%s", bookAuthor);
		
		printf("Which row would you like to place your entry in?\n");
		scanf("%d", &row);

		printf("You would like to place the book titles: %s by author: %s in row: %d\n",bookTitle, bookAuthor, row);

		int bytePosition = getBytePosition(row, fileDesc);
		printf("Byte position is %d\n", bytePosition);

		insertBook(fileDesc, bytePosition, bookTitle, bookAuthor);

		if(bytesRead = read(fileDesc, readData, sizeof(readData)) == -1)
			printf("Error reading from file %s\n", strerror(errno));
		else
			printf("The contents of the book.txt file are: \n%s\n", readData);
	}

	else
	{	printf("Okay, goodbye!\n");
	}
	
	/*********************************************************************************************/

	return 0;
}
int getBytePosition(int row, int filedes)
{	printf("In getBytePosition()....\n");
	char buf[200];
	int i;

	ssize_t num_bytes=lseek(filedes, 0, SEEK_END);
	printf("num_bytes=%zd\n", num_bytes);

	lseek(filedes, 0, SEEK_SET);

	ssize_t read_bytes=read(filedes, buf, num_bytes);
	printf("Read bytes=%zd\n", read_bytes);

	int num_rows = 1;
	for(int i=0; i < num_bytes; i++)
	{	//printf("buf[%d]=%c\n", i, buf[i]);

		if(buf[i] == '\n')
		{	printf("***EOL\n");
			num_rows++;

			if(num_rows==row)
				return i+1;
		}
	}
	
	return -1;
}

void insertBook(int fd, int byte, char *BookTitle, char *authorName)
{	char readData[4028] = {0};
	char buffer[4028] = {0};
	int bytesRead, bytesWritten;

	lseek(fd, 0, SEEK_SET);

	if(bytesRead = read(fd, readData, sizeof(readData)) == -1)
		printf("Error reading from file %s\n", strerror(errno));
	else
		printf("The contents of the book.txt file are: \n%s\n", readData);

	for(int i = byte, j = 0; i < strlen(readData); i++, j++)
		buffer[j] = readData[i];

	printf("Buffer contents %s\n", buffer);

	lseek(fd, byte, SEEK_SET);

	if(bytesWritten = write(fd, BookTitle, strlen(BookTitle)) < strlen(BookTitle))
		fprintf(stderr, "Error writing to file: %s\n", strerror(errno));

	if(bytesWritten = write(fd, authorName, strlen(authorName)) < strlen(authorName))
		fprintf(stderr, "Error writing to file: %s\n", strerror(errno));

	if(bytesWritten = write(fd, buffer, strlen(buffer)) < strlen(buffer))
		fprintf(stderr, "Error writing to file: %s\n", strerror(errno));

}
