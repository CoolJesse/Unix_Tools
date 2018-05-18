#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{	
	char name[256] = {0};
	char cwd[1024] = {0};
	char *fileName = "myName.txt";
	char *user_prompt = "Please enter a name to write to the file> ";

	int fd;

	write(STDOUT_FILENO, user_prompt, strlen(user_prompt));

	while(read(STDIN_FILENO, name, 256) == 0)
	{
		printf("\n");		
		write(STDOUT_FILENO, user_prompt, strlen(user_prompt));
	}
	/****************************************************************************/
	/** Gets and prints current directory ***************************************/	

	if(getcwd(cwd, sizeof(cwd)) == NULL)
		perror("getcwd() error");
	/***************************************************************************/
	/** appends the file name of the file to write to, to the path **/

	int i = 0;
	for(; cwd[i] != 0; i++)
		;
	cwd[i] = '/';
	i++;

	for(int j = 0;j < strlen(fileName); j++, i++)
		cwd[i] = fileName[j];
	cwd[i] = '\0';				

	printf("Writing to directory: %s\n", cwd);

	/***************************************************************************/
	/** Open file to write to **************************************************/
	fd = open(cwd, O_RDWR|O_CREAT|O_TRUNC);
	/**************************************************************************/
	/** Write to file *********************************************************/
	if(write(fd, name, strlen(name)) != strlen(name))
		printf("Error writing to file.");
	/****************************************************************************/
	
	return 0;
}
