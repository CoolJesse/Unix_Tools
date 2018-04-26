
/*******************************************************************************
********************************************************************************
Macros defined in /usr/include/sys/stat.h

#define S_IFMT 0170000

#define S_ISBLK(m) (((mode) & S_IFMT) == S_IFBLK)
#define S_ISCHR(m) (((mode) & S_IFMT) == S_IFCHR)
#define S_ISDIR(m) (((mode) & S_IFMT) == S_IFDIR)
#define S_ISFIFO(m) (((mode) & S_IFMT) == S_IFFIFO)
#define S_ISREG(m) (((mode) & S_IFMT) == S_IFREG)
#define S_ISLINK(m) (((mode) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m) (((mode) & S_IFMT) == S_IFSOCK)

S_IFMT = (0170000)octal = (001 111 000 000 000 000)binary
S_IFBLK = (060000)octal = (001 100 000 000 000 000)binary

		     001 100 000 000 000 000
S_IFBLK ^ S_IFMT = ^ 001 111 000 000 000 000
		   ---------------------------
		     001 100 000 000 000 000
*******************************************************************************
*******************************************************************************/

#include <sys/types.h>
#include <sys/stat.h> //for filetype macros and mask S_IFMT
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void file_mode(struct stat buf, char *ptr, char *binary_value);
char* oct_to_binary(char* octal_value);

int main(int argc, char *argv[])
{
	int i;
	struct stat buf; //for reading stat() function results
	char *ptr, *binary_value;

	for(i=1; i<argc; i++)
	{	
		printf("%s: \n", argv[i]);

		if( stat(argv[i], &buf) < 0)
		{
			printf("stat() error\n");
			continue;
		}

		file_mode(buf, ptr, binary_value);
	}	

	return 0;
}
void file_mode(struct stat buf, char *ptr, char *binary_value)
{
	char *test_string = "|\tst_nlink\t\t|\t%lo\t\t\t|\n";

	printf("S_IFMT = %o\n", S_IFMT);
	printf("st_mode & S_IFMT = %o\n", buf.st_mode & S_IFMT);
	printf("\n");

	for(int i=0; i < strlen(test_string); i++)
	{
		if(test_string[i] == '\t')
			printf("------");
		else
			printf("-");
	}
	printf("\n");
	
	printf("struct stat:\n");
	printf("|\tst_mode \t\t|\t%o\t\t|\n", buf.st_mode);
	printf("|\tst_ino  \t\t|\t%lo\t\t|\n", buf.st_ino);
	printf("|\tst_dev  \t\t|\t%lo\t\t|\n", buf.st_dev);
	printf("|\tst_rdev \t\t|\t%lo\t\t|\n", buf.st_rdev);
	printf("|\tst_nlink\t\t|\t%lo\t\t|\n", buf.st_nlink);
	printf("|\tst_uid  \t\t|\t%o\t\t|\n", buf.st_uid);
	printf("|\tst_gid  \t\t|\t%o\t\t|\n", buf.st_gid);
	printf("|\tst_size \t\t|\t%lo\t\t|\n", buf.st_size);
	printf("|\tst_atime\t\t|\t%lo\t\t|\n", buf.st_atime);

	for(int i=0; i < strlen(test_string); i++)
	{
		if(test_string[i] == '\t')
			printf("------");
		else
			printf("-");
	}
	printf("\n");
	
	if( S_ISREG(buf.st_mode) )
		ptr = "regular";
	else if( S_ISDIR(buf.st_mode) )
		ptr = "directory";
	else if( S_ISCHR(buf.st_mode) )
		ptr = "character special";
	else if( S_ISBLK(buf.st_mode) )
		ptr = "block special";
	else if( S_ISFIFO(buf.st_mode) )
		ptr = "FIFO";
	else if( S_ISLNK(buf.st_mode) )
		ptr = "symbolic link";
	else if( S_ISSOCK(buf.st_mode) )
		ptr = "socket";
	else
		ptr = "unknown file mode";
		printf("File Type: %s\n", ptr);

	printf("\n");

	char octal_value[11];
	char *temp;

	int mode_int = buf.st_mode;
	sprintf(octal_value, "%d", mode_int);

	printf("octal value %s\n", octal_value );

	temp = oct_to_binary(octal_value);

	printf("binary value %s\n", temp);
}

char* oct_to_binary(char* octal_value)
{
	int binary_length = strlen(octal_value)*3;
	
	char* binary_value = (char*)malloc(binary_length*sizeof(char));

	/*Loop for passing through octal_value and creating binary_values string*/
	/*i variable corresponds to octal_value and j to binary_value*/
	for(int i=0, j=0; i < strlen(octal_value); i++, j+=3)
	{				
		if(octal_value[i] == '0')
		{
			binary_value[j] = '0';
			binary_value[j+1] = '0';
			binary_value[j+2] = '0'; 
		}
		else if(octal_value[i] == '1')
		{
			binary_value[j] = '0';
			binary_value[j+1] = '0';
			binary_value[j+2] = '1'; 
		}
		else if(octal_value[i] == '2')
		{
			binary_value[j] = '0';
			binary_value[j+1] = '1';
			binary_value[j+2] = '0'; 
		} 
		else if(octal_value[i] == '3')
		{
			binary_value[j] = '0';
			binary_value[j+1] = '1';
			binary_value[j+2] = '1'; 
		}
		else if(octal_value[i] == '4')
		{
			binary_value[j] = '1';
			binary_value[j+1] = '0';
			binary_value[j+2] = '0'; 
		}
		else if(octal_value[i] == '5')
		{
			binary_value[j] = '1';
			binary_value[j+1] = '0';
			binary_value[j+2] = '1'; 
		}
		else if(octal_value[i] == '6')
		{
			binary_value[j] = '1';
			binary_value[j+1] = '1';
			binary_value[j+2] = '0'; 
		}
		else if(octal_value[i] == '7')
		{
			binary_value[j] = '1';
			binary_value[j+1] = '1';
			binary_value[j+2] = '1'; 
		}
		else
		{
			binary_value[j] = '?';
			binary_value[j+1] = '?';
			binary_value[j+2] = '?'; 
		}
	}

	return binary_value;


}
