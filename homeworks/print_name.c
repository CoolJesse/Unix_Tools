#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{	
	if(argc < 2)
		printf("You failed to enter a name to print\n");
	
	else
	{	char cwd[1024];
		if(getcwd(cwd, sizeof(cwd)) != NULL)
			fprintf(stdout, "Current working directory: %s\n", cwd);
		else
			perror("getcwd() error");	

		int totalLength = 0;

		for(int i = 1; i < argc; i++)
			totalLength += strlen(argv[i]);

		//printf("%d", totalLength);
		totalLength = totalLength+1+(argc-2);//account for spaces and \n at end
		char name[totalLength];
		
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

		//print contents of name buffer
		for(int i = 0; i < totalLength; i++)
			printf("%c", name[i]);
	}
	
	return 0;
}
