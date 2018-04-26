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

void file_stat(struct stat buf);
char* oct_to_binary(char* octal_value);

int main(int argc, char *argv[])
{
	int i;
	struct stat buf; //for reading stat() function results

	for(i=1; i<argc; i++)
	{	
		printf("%s: \n", argv[i]);

		if( stat(argv[i], &buf) < 0)
		{
			printf("stat() error\n");
			continue;
		}

		file_stat(buf);
	}	

	return 0;
}
void file_stat(struct stat buf)
{
	char *test_string = "|\tst_nlink\t\t|\t%lo\t\t\t|\n";
	char *ptr;

	char octal_value[32];
	char *binary_value;

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
	
	/*
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
	*/




	int temp_int;
	long int temp_long_int;

	printf("struct stat:\n");
	
	/* st_mode ****************************************/
	sprintf(octal_value, "%o", buf.st_mode);
	binary_value = oct_to_binary(octal_value);

	printf("|\tst_mode \t\t|\t%s\t\t|\n", binary_value);
	//printf("|\tst_mode \t\t|\t%o\t\t|\n", buf.st_mode);

	/* st_ino ******************************************/
	sprintf(octal_value, "%lo", buf.st_ino);
	//binary_value = oct_to_binary(octal_value);

	printf("|\tst_ino  \t\t|\t%s\t\t|\n", binary_value);
	//printf("|\tst_ino  \t\t|\t%lo\t\t|\n", buf.st_ino);

	/* st_dev ******************************************/
	sprintf(octal_value, "%lo", buf.st_dev);
	//binary_value = oct_to_binary(octal_value);

	printf("|\tst_dev  \t\t|\t%s\t\t|\n", binary_value);
	//printf("|\tst_dev  \t\t|\t%lo\t\t|\n", buf.st_dev);

	/* sr_rdev ****************************************/
	sprintf(octal_value, "%lo", buf.st_rdev);
	binary_value = oct_to_binary(octal_value);

	printf("|\tst_rdev \t\t|\t%s\t\t|\n", binary_value);
	//printf("|\tst_rdev \t\t|\t%lo\t\t|\n", buf.st_rdev);

	/* st_link ****************************************/
	sprintf(octal_value, "%lo", buf.st_nlink);
	binary_value = oct_to_binary(octal_value);

	printf("|\tst_nlink\t\t|\t%s\t\t|\n", binary_value);
	//printf("|\tst_nlink\t\t|\t%lo\t\t|\n", buf.st_nlink);

	/* st_uid *****************************************/
	sprintf(octal_value, "%o", buf.st_uid);
	binary_value = oct_to_binary(octal_value);

	printf("|\tst_uid  \t\t|\t%s\t\t|\n", binary_value);
	//printf("|\tst_uid  \t\t|\t%o\t\t|\n", buf.st_uid);

	/* st_gid *****************************************/
	sprintf(octal_value, "%o", buf.st_gid);
	binary_value = oct_to_binary(octal_value);

	printf("|\tst_gid  \t\t|\t%s\t\t|\n", binary_value);
	//printf("|\tst_gid  \t\t|\t%o\t\t|\n", buf.st_gid);

	/* st_size ****************************************/
	sprintf(octal_value, "%lo", buf.st_size);
	binary_value = oct_to_binary(octal_value);

	printf("|\tst_size \t\t|\t%s\t\t|\n", binary_value);
	//printf("|\tst_size \t\t|\t%lo\t\t|\n", buf.st_size);

	/* st_atime ***************************************/
	sprintf(octal_value, "%lo", buf.st_atime);
	binary_value = oct_to_binary(octal_value);

	printf("|\tst_atime\t\t|\t%s\t\t|\n", binary_value);
	//printf("|\tst_atime\t\t|\t%lo\t\t|\n", buf.st_atime);

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
}

char* oct_to_binary(char* octal_value)
{
	//int binary_length = strlen(octal_value)*3;
	char* binary_value = (char*)malloc(32*sizeof(char));

	int j =0;
	if(strlen(octal_value) < 11)
	{	//printf("octal_value: %ld\n", strlen(octal_value));
		int append_length = ( 32 - (strlen(octal_value)*3) );
		for( ; j < append_length; j++)
			binary_value[j] = '0';
	}
	
	/*Loop for passing through octal_value and creating binary_values string*/
	/*i variable corresponds to octal_value and j to binary_value*/
	for(int i=0; i < strlen(octal_value); i++, j+=3)
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

	}

	return binary_value;


}
