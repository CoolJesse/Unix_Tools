#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <inttypes.h>

#define MAX_PATH_LENGTH 1024

int main()
{
/** Get the current working directory ******************************************************************/

	char current_working_directory[MAX_PATH_LENGTH] = {0};

	if(getcwd(current_working_directory, sizeof(current_working_directory)) != NULL)
		printf("Current working directory: %s\n", current_working_directory);
	else
		perror("getcwd() error");
/******************************************************************************************************/
/** Declare variables *********************************************************************************/
	int fileDesc;
	char *fileName1 = "/file1.txt";
	char *fileName2 = "/file2.txt";
	char *fileName3 = "/file3.txt";

	char path_name1[256] = {0};
	char path_name2[256] = {0};
	char path_name3[256] = {0};

/******************************************************************************************************/	
/** appends the file name of the file to write to the path ********************************************/
	int i = 0;
	for(; current_working_directory[i] != 0; i++)
	{
		path_name1[i] = current_working_directory[i];
		path_name2[i] = current_working_directory[i];
		path_name3[i] = current_working_directory[i];
	}

	for(int j=0, k=i; j < strlen(fileName1) && k < MAX_PATH_LENGTH; j++, k++)
		path_name1[i] = fileName1[j];

	for(int j=0, k=i; j < strlen(fileName2) && k < MAX_PATH_LENGTH; j++, k++)
		path_name2[i] = fileName2[j];

	for(int j=0, k=i; j < strlen(fileName3) && k < MAX_PATH_LENGTH; j++, k++)
		path_name3[i] = fileName3[j];
/****************************************************************************************************/
/** Open files **************************************************************************************/	
	if((fileDesc = open(path_name1, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) == -1)
		fprintf(stderr, "Error opening/creating file1: %s\n", strerror(errno));
	else
		printf("File1 opened/created successfully.\n");	 

	if((fileDesc = open(path_name2, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP)) == -1)
		fprintf(stderr, "Error opening/creating file2: %s\n", strerror(errno));
	else
		printf("File2 opened/created successfully.\n");

	if((fileDesc = open(path_name3, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
		fprintf(stderr, "Error opening/creating file3: %s\n", strerror(errno));
	else
		printf("File3 opened/created successfully.\n");
/***************************************************************************************************/
/** End of main program ****************************************************************************/
	return 0;
}
