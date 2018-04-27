#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <inttypes.h>

int main()
{
	/**************************** Get the current working directory *****************************/
	int fileDesc;
	char *fileName1 = "/file1.txt";
	char *fileName2 = "/file2.txt";
	char *fileName3 = "/file3.txt";
	char path_name1[1024] = {0};
	char path_name2[1024] = {0};
	char path_name3[1024] = {0};
		
	if(getcwd(path_name1, sizeof(path_name1)) != NULL)
		printf("Current working directory: %s\n", path_name1);
	else
		perror("getcwd() error");

	if(getcwd(path_name2, sizeof(path_name2)) != NULL)
		printf("Current working directory: %s\n", path_name2);
	else
		perror("getcwd() error");

	if(getcwd(path_name3, sizeof(path_name3)) != NULL)
		printf("Current working directory: %s\n", path_name3);
	else
		perror("getcwd() error");

	/********************************************************************************************/

	/************* appends the file name of the file to write to the path ***********************/
	int i = 0;
	for(; path_name1[i] != 0; i++)
			; //empty for loop to pass through character array

	for(int j = 0;j < strlen(fileName1); j++, i++)
		path_name1[i] = fileName1[j];

	path_name1[i] = '\n';	
	/*******************************************************************************************/
	i = 0;
	for(; path_name2[i] != 0; i++)
			;

	for(int j = 0;j < strlen(fileName2); j++, i++)
		path_name2[i] = fileName2[j];

	/********************************************************************************************/
	/*******************************************************************************************/
	i = 0;
	for(; path_name3[i] != 0; i++)
			;

	for(int j = 0;j < strlen(fileName3); j++, i++)
		path_name3[i] = fileName3[j];

	/********************************************************************************************/

	/*********************************** Create file to write to **********************************/
	//mode_t mode = S_IRWXU; //This is equivalent to ‘(S_IRUSR | S_IWUSR | S_IXUSR)


	if((fileDesc = open(path_name1, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR)) == -1)
		fprintf(stderr, "Error opening/creating file1: %s\n", strerror(errno));
	else
		printf("File1 opened/created successfully.\n");	

	if((fileDesc = open(path_name2, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP)) == -1)
		fprintf(stderr, "Error opening/creating file1: %s\n", strerror(errno));
	else
		printf("File1 opened/created successfully.\n");

	if((fileDesc = open(path_name3, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR)) == -1)
		fprintf(stderr, "Error opening/creating file1: %s\n", strerror(errno));
	else
		printf("File1 opened/created successfully.\n");



	return 0;
}
