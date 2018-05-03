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

void file_stat(char *file_name, struct stat buf);
void permission_bits(char *file_name, struct stat buf);
void file_type(char *file_name, struct stat buf);
void sticky_bit(char *file_name, struct stat buf);
void set_group_id(char *file_name, struct stat buf);
void set_user_id(char *file_name, struct stat buf);

char* oct_to_binary(char* octal_value);
void display_permission_bit_masks(void);

int main(int argc, char *argv[])
{
/************************************************************************************************/
/** Main program, read stat info into buffer and display contents *******************************/
	struct stat buf; //for reading stat() function results

	for(int i=1; i<argc; i++)
	{	
		printf("%s: \n", argv[i]);

		if( stat(argv[i], &buf) < 0)
		{
			printf("stat() error\n");
			continue;
		}

		file_stat(argv[i], buf);
		display_permission_bit_masks();
		permission_bits(argv[i], buf);
		file_type(argv[i], buf);
		sticky_bit(argv[i], buf);
		set_group_id(argv[i], buf);
		set_user_id(argv[i], buf);
	}	
/**********************************************************************************************/
/** End of main program ***********************************************************************/
	return 0;
}
/**********************************************************************************************/
/* Display the values of the stat structure of the file ***************************************/
void file_stat(char * file_name, struct stat buf)
{
	char octal_value[32];
	char *binary_value;

	printf("%s struct stat:\n", file_name);
	printf("--------------------------------------------------------------------------------\n");
	
	/* st_mode ****************************************/
	sprintf(octal_value, "%o", buf.st_mode);
	binary_value = oct_to_binary(octal_value);
	printf("|\tst_mode \t\t|\t%s\t\t|\n", binary_value);

	//printf("|\tst_mode \t\t|\t%o\t\t|\n", buf.st_mode); //octal representation of the value

	/* st_ino ******************************************/
	sprintf(octal_value, "%lo", buf.st_ino);
	binary_value = oct_to_binary(octal_value);
	printf("|\tst_ino  \t\t|\t%s\t\t|\n", binary_value);

	//printf("|\tst_ino  \t\t|\t%lo\t\t|\n", buf.st_ino);

	/* st_dev ******************************************/
	sprintf(octal_value, "%lo", buf.st_dev);
	binary_value = oct_to_binary(octal_value);
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

	printf("--------------------------------------------------------------------------------\n");
	printf("\n");
}
/***********************************************************************************************/
/** Display the binary values for each st_mode bit mask ****************************************/
void display_permission_bit_masks(void)
{
	char octal_value[32];
	char *binary_value;

	printf("File permission bits masks:\n");
	printf("--------------------------------------------------------------------------------\n");

	/* S_IRUSR ****************************************/
	sprintf(octal_value, "%o", S_IRUSR);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IRUSR: user-read\t|\t%s\t\t|\n", binary_value);

	/* S_IWUSR ****************************************/
	sprintf(octal_value, "%o", S_IWUSR);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IWUSR: user-write\t|\t%s\t\t|\n", binary_value);

	/* S_IXUSR ****************************************/
	sprintf(octal_value, "%o", S_IXUSR);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IXUSR: user-execute\t|\t%s\t\t|\n", binary_value);

	/* S_IRGRP ***************************************/
	sprintf(octal_value, "%o", S_IRGRP);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IRGRP: group-read\t|\t%s\t\t|\n", binary_value);

	/* S_IWGRP ****************************************/
	sprintf(octal_value, "%o", S_IWGRP);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IWGRP: group-write\t|\t%s\t\t|\n", binary_value);

	/* S_IXGRP ****************************************/
	sprintf(octal_value, "%o", S_IXGRP);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IXGRP: group_execute\t|\t%s\t\t|\n", binary_value);

	/* S_IROTH ****************************************/
	sprintf(octal_value, "%o", S_IROTH);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IROTH: other_read\t|\t%s\t\t|\n", binary_value);

	/* S_IWOTH ****************************************/
	sprintf(octal_value, "%o", S_IWOTH);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IWOTH: other_write\t|\t%s\t\t|\n", binary_value);

	/* S_IXOTH ****************************************/
	sprintf(octal_value, "%o", S_IXOTH);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IXOTH: other-execute\t|\t%s\t\t|\n", binary_value);

	printf("--------------------------------------------------------------------------------\n");	
	printf("\n");
}
/************************************************************************************************/
/* st_mode permission bit values for the file ***************************************************/
void permission_bits(char *file_name, struct stat buf)
{
	char octal_value[32];
	char *binary_value;

	printf("%s st_mode permission bits:\n", file_name);
	printf("--------------------------------------------------------------------------------\n");
	/* st_mode *********************************************************/
	sprintf(octal_value, "%o", buf.st_mode);
	binary_value = oct_to_binary(octal_value);
	printf("|\tst_mode                  |\t%s\t\t|\n", binary_value);
	printf("-------------------------------------------------------------------------------\n");
	/* S_IRUSR *********************************************************/
	sprintf(octal_value, "%o", (S_IRUSR & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tUser read permission:    |\t%s\t\t|\n", binary_value);

	/* S_IWUSR *********************************************************/
	sprintf(octal_value, "%o", (S_IWUSR & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tUser write permission:   |\t%s\t\t|\n", binary_value);

	/* S_IXUSR *********************************************************/
	sprintf(octal_value, "%o", (S_IXUSR & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tUser execute permission: |\t%s\t\t|\n", binary_value);

	/* S_IRGRP *********************************************************/
	sprintf(octal_value, "%o", (S_IRGRP & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tGroup read permission:   |\t%s\t\t|\n", binary_value);

	/* S_IWGRP *********************************************************/
	sprintf(octal_value, "%o", (S_IWGRP & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tGroup write permission:  |\t%s\t\t|\n", binary_value);

	/* S_IXGRP *********************************************************/
	sprintf(octal_value, "%o", (S_IXGRP & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tGroup execute permission:|\t%s\t\t|\n", binary_value);

	/* S_IROTH *********************************************************/
	sprintf(octal_value, "%o", (S_IROTH & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tOther read permission:   |\t%s\t\t|\n", binary_value);

	/* S_IWOTH *********************************************************/
	sprintf(octal_value, "%o", (S_IWOTH & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tOther write permission:  |\t%s\t\t|\n", binary_value);

	/* S_IXOTH *********************************************************/
	sprintf(octal_value, "%o", (S_IXOTH & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tOther execute permission:|\t%s\t\t|\n", binary_value);

	printf("--------------------------------------------------------------------------------\n");
	printf("\n");
}
/************************************************************************************************/
/** Outputs filetype bit octal and binary values for file and tests for type of file **********/
void file_type(char *file_name, struct stat buf)
{
	char *ptr;
	char octal_value[32];
	char *binary_value;

/* File Type *********************************************************************************************/

	printf("%s st_mode filetype bits:\n", file_name);
	printf("-------------------------------------------------------------------------------\n");
	/* st_mode **********************************************************************************/	
	sprintf(octal_value, "%o", (buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tst_mode: |\toctal: %o binary: %s\t\t|\n", buf.st_mode, binary_value);

	/* S_IFMT **********************************************************************************/
	sprintf(octal_value, "%o", (S_IFMT));
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IFMT:  |\toctal: %o binary: %s\t\t|\n", S_IFMT, binary_value);

	printf("-------------------------------------------------------------------------------\n");

	sprintf(octal_value, "%o", (S_IFMT & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tbuf.st_mode & S_IFMT:  %o \t      %s\t\t|\n", (S_IFMT & buf.st_mode), binary_value);

	printf("-------------------------------------------------------------------------------\n");

	/* S_IFBLK *********************************************************************************/
	sprintf(octal_value, "%o", (S_IFBLK));
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IFBLK: |\toctal: %o binary:  %s\t\t|\n", S_IFBLK, binary_value);

	/* S_IFCHR *********************************************************************************/
	sprintf(octal_value, "%o", (S_IFCHR));
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IFCHR: |\toctal: %o binary:  %s\t\t|\n", S_IFCHR, binary_value);

	/* S_IFDIR *********************************************************************************/
	sprintf(octal_value, "%o", (S_IFDIR));
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IFDIR: |\toctal: %o binary:  %s\t\t|\n", S_IFDIR, binary_value);

	/* S_IFIFO ********************************************************************************/
	sprintf(octal_value, "%o", (S_IFIFO));
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IFIFO: |\toctal: %o binary:  %s\t\t|\n", S_IFIFO, binary_value);

	/* S_IFREG ********************************************************************************/
	sprintf(octal_value, "%o", (S_IFREG));
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IFREG: |\toctal: %o binary: %s\t\t|\n", S_IFREG, binary_value);

	/* S_IFLNK ********************************************************************************/
	sprintf(octal_value, "%o", (S_IFLNK));
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IFLNK: |\toctal: %o binary: %s\t\t|\n", S_IFLNK, binary_value);

	/* S_IFSOCK ******************************************************************************/
	sprintf(octal_value, "%o", (S_IFSOCK));
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_IFSOCK:|\toctal: %o binary: %s\t\t|\n", S_IFSOCK, binary_value);

	printf("-------------------------------------------------------------------------------\n");

	/* Use macros included in sys.stat.h to test for file type *******************************/
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

	printf("%s file type: %s\n", file_name, ptr);
	printf("-------------------------------------------------------------------------------\n");
	printf("\n");
}
/**************************************************************************************************/
/** Test to see if sticky bit is set and outputs results in octal and binary **********************/
void sticky_bit(char *file_name, struct stat buf) // 0000 0010 0000 0000 10th bit
{
	char octal_value[32];
	char *binary_value;	

	printf("%s sticky bit:\n", file_name);
	printf("-------------------------------------------------------------------------------\n");	
	sprintf(octal_value, "%o", S_ISVTX);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_ISVTX: |\toctal: %o   binary: %s\t\t|\n", S_ISVTX, binary_value);

	printf("-------------------------------------------------------------------------------\n");
	sprintf(octal_value, "%o", (buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tst_mode: |\toctal: %o binary: %s\t\t|\n", buf.st_mode, binary_value);

	printf("-------------------------------------------------------------------------------\n");	
	sprintf(octal_value, "%o", (S_ISVTX & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|   st_mode & S_ISVTX: |  octal: %o    binary: %s\t\t|\n", (S_ISVTX & buf.st_mode), binary_value);
	printf("-------------------------------------------------------------------------------\n");
}
/**************************************************************************************************/
/** when set, S_ISGID bit allows user to run executable with permission level of executable's *****/
/** group and change behavior in directories ******************************************************/
void set_group_id(char *file_name, struct stat buf) //0000 0100 0000 0000 11th bit
{
	char octal_value[32];
	char *binary_value;

	printf("%s group id:\n", file_name);
	printf("-------------------------------------------------------------------------------\n");	
	sprintf(octal_value, "%o", S_ISGID);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_ISGID: |\toctal: %o   binary: %s\t\t|\n", S_ISGID, binary_value);

	printf("-------------------------------------------------------------------------------\n");
	sprintf(octal_value, "%o", (buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tst_mode: |\toctal: %o binary: %s\t\t|\n", buf.st_mode, binary_value);

	printf("-------------------------------------------------------------------------------\n");	
	sprintf(octal_value, "%o", (S_ISGID & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|   st_mode & S_ISGID: |  octal: %o    binary: %s\t\t|\n", (S_ISGID & buf.st_mode), binary_value);
	printf("-------------------------------------------------------------------------------\n");
}
/*************************************************************************************************/
/** when set, S_ISUID bit allows user to run executable with permission level of executable's ****/
/** owner and change behavior in directories *****************************************************/
void set_user_id(char *file_name, struct stat buf) // 0000 1000 0000 0000 12th bit
{
	char octal_value[32];
	char *binary_value;

	printf("%s user id:\n", file_name);
	printf("-------------------------------------------------------------------------------\n");	
	sprintf(octal_value, "%o", S_ISUID);
	binary_value = oct_to_binary(octal_value);
	printf("|\tS_ISUID: |\toctal: %o   binary: %s\t\t|\n", S_ISUID, binary_value);

	printf("-------------------------------------------------------------------------------\n");
	sprintf(octal_value, "%o", (buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|\tst_mode: |\toctal: %o binary: %s\t\t|\n", buf.st_mode, binary_value);

	printf("-------------------------------------------------------------------------------\n");	
	sprintf(octal_value, "%o", (S_ISUID & buf.st_mode));
	binary_value = oct_to_binary(octal_value);
	printf("|   st_mode & S_ISUID: |  octal: %o    binary: %s\t\t|\n", (S_ISUID & buf.st_mode), binary_value);
	printf("-------------------------------------------------------------------------------\n");
}
/************************************************************************************************/
/** Function takes octal input and returns a string binary equivalent 32 bytes long *************/
char* oct_to_binary(char* octal_value)
{
	char* binary_value = (char*)malloc(33*sizeof(char)); //extra byte for '\0'character

	int j =0;
	if(strlen(octal_value) < 11)
	{	
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
	binary_value[32] = '\0';
	return binary_value;
}
/******************************************************************************************/
/** End of program ************************************************************************/
