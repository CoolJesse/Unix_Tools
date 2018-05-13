#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{	
	if(argc < 2)
		printf("You failed to enter a name to print\n");
	
	else
	{	
		/****************************************************************************/
		/** Gets and prints current directory **/
		char *fileName = "/myName.txt";
		char cwd[1024] = {0};
		//int dirLength = 0;
		if(getcwd(cwd, sizeof(cwd)) != NULL)
			printf("Current working directory: %s\n", cwd);
		else
			perror("getcwd() error");
		/***************************************************************************/
		/** appends the file name of the file to write to, to the path **/
		int i = 0;
		for(; cwd[i] != 0; i++)
			;
		for(int j = 0;j < 11; j++, i++)
			cwd[i] = fileName[j];
		cwd[i] = '\n';				

		printf("The directory to write to is: %s", cwd);
		/***************************************************************************/
		/** Determines total length of name passed as command line arguement **/		
		int totalLength = 0;

		for(int i = 1; i < argc; i++)
			totalLength += strlen(argv[i]);

		totalLength = totalLength+1+(argc-2);//account for spaces and \n at end
		char name[totalLength];
		/***************************************************************************/
		/** Populates name buffer with name passed as command line argument **/
		int h = 0;		
		for(int i=1; i < argc; i++)
		{

			int length = strlen(argv[i]);

			for(int j = 0; j < length; h++, j++)
				name[h] = argv[i][j];
			name[h] = ' ';
			h++;	
		}
		name[totalLength-1] = '\n';
		/****************************************************************************/
		/** print contents of name buffer **/
		for(int i = 0; i < totalLength; i++)
			printf("%c", name[i]);
		/****************************************************************************/
		/** creates a file to write to **/
		int fd;
		mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
		//char *filename = ;
		fd = creat(cwd, mode);
		printf("File descriptor is %d, total length is %d\n", fd, totalLength);
		//fd = creat(filename, mode);
		/****************************************************************************/
		/** Write name to file **/
		printf("My name is %s", name);

		if(write(fd, name, totalLength) != totalLength)
			printf("Error writing to file.");
		/****************************************************************************/
		
	}
	
	return 0;
}
