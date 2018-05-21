/*
                      |a b c|   |j k|   |(a*j+b*l+c*o) (a*k+b*m+c*p)|   |aj+bl+co ak+bm+cp|
Matrix Multiplication:|d e f| x |l m| = |(d*j+e*l+f*o) (d*k+e*m+f*p)| = |dj+el+fo dk+em+fp|
                      |g h i|   |o p|   |(g*j+h*l+i*o) (g*k+h*m+i*p)|   |gj+hl+io gk+hm+ip|
*/

#include <pthread.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h> 

#define row_A_size 3
#define column_A_size 4

#define row_B_size 4
#define column_B_size 3

void *dot_product(void *arguments);

pthread_t thread_1, thread_2, thread_3;
void *tret1, *tret2, *tret3, *tret4;
pthread_mutex_t *lock;

/********************************************************************/
struct matrix_multiplication_struct
{
	int (*array1)[row_A_size][column_A_size];
	int (*array2)[row_B_size][column_B_size];
	int (*array3)[row_A_size][column_B_size];

	int row_index; //row index of array1, column index of array2
};
/********************************************************************/

int main(void)
{
	int array_A[row_A_size][column_A_size] = { {1,2,3,4},{5,6,7,8},{9,10,11,12} };
	int array_B[row_B_size][column_B_size] = { {2,3,5},{7,11,13},{17,19,23},{29,31,37} };

	int array_C[row_A_size][column_B_size];
	int err, err1, err2;

	/*********************************************************************************/
	/** Print contents of matix_A ****************************************************/
	printf("matrix_A:\n");
	for(int i=0; i<row_A_size; i++)
	{
		for(int j=0; j<column_A_size; j++)
		{
			printf("%d ", array_A[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	
	/********************************************************************************/
	/** Print contents of matrix_B **************************************************/
	printf("matrix_B:\n");
	for(int i=0; i<row_B_size; i++)
	{
		for(int j=0; j<column_B_size; j++)
		{
			printf("%d ", array_B[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	/********************************************************************************/
	/** Create struct to pass multiple arguments to pthread_create() ***************/
	struct matrix_multiplication_struct arguments;

	arguments.array1 = &array_A;
	arguments.array2 = &array_B;
	arguments.array3 = &array_C;
	/*******************************************************************************/
	/** Test arguments->arrayx *****************************************************/
	printf("matrix_multiplication_struct arguments->array1:\n");
	for(int i=0; i<row_A_size; i++)
	{
		for(int j=0; j<column_A_size; j++)
		{
			printf("%d ", (*arguments.array1)[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	printf("matrix_multiplication_struct arguments->array2:\n");
	for(int i=0; i<row_B_size; i++)
	{
		for(int j=0; j<column_B_size; j++)
		{
			printf("%d ", (*arguments.array2)[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	/********************************************************************************/
	/** initilize thread lock *******************************************************/
	lock = malloc(sizeof(pthread_mutex_t));
	err=pthread_mutex_init(lock, NULL);
	if(err!=0)
		printf("Could not create mutex, err=%s.\n", strerror(err));
	/********************************************************************************/
	/** Create threads and join them with main thread *******************************/
	printf("creating  threads and joining them with main() thread.................\n");
	printf("\n");
	/** thread 1 ********************************************************/

	arguments.row_index = 0; 
	err=pthread_create(&thread_1, NULL, dot_product, (void *)&arguments);
	if(err !=0)
		printf("Error creating thread: %d\n", err);

	err2=pthread_join(thread_1, &tret1);
	if(err2 !=0)
		printf("Error joining thread: %d\n", err);

	/** thread 2 ********************************************************/

	arguments.row_index = 1; 
	err = pthread_create(&thread_2, NULL, dot_product, (void *)&arguments);
	if(err !=0)
		printf("Error creating thread: %d\n", err);

	err2 = pthread_join(thread_2, &tret2);
	if(err2 !=0)
		printf("Error joining thread: %d\n", err);

	/** thread 3 ********************************************************/

	arguments.row_index = 2; 
	err = pthread_create(&thread_3, NULL, dot_product, (void *)&arguments);
	if(err !=0)
		printf("Error creating thread: %d\n", err);

	err2 = pthread_join(thread_3, &tret3);
	if(err2 !=0)
		printf("Error joining thread: %d\n", err);

	/********************************************************************************/
	/** print exit codes of each thread *********************************************/

	//printf("Thread %d has exit code %ld.\n", (int)thread_1, (long)tret1);
	//printf("Thread %d has exit code %ld.\n", (int)thread_2, (long)tret2);
	//printf("Thread %d has exit code %ld.\n", (int)thread_3, (long)tret3);

	//printf("\n");
	/********************************************************************************/
	/** Print contents of matrix_C **************************************************/
	printf("matrix_C:\n");
	for(int i=0; i<row_A_size; i++)
	{
		for(int j=0; j<column_B_size; j++)
		{
			printf("%d ", array_C[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	/********************************************************************************/
	return 0;
}

/** Function performs dot product multiplication or given row from array1***************************************/

void *dot_product(void *arguments)
{
	int err1;

	/** Thread lock to prevent arguments.row_index from being altered before function finishes executing **/
	err1 = pthread_mutex_lock(lock);
	if(err1 !=0)
		printf("Error locking mutex, err=%s.\n", strerror(err1));
	/*****************************************************************************************************/

	struct matrix_multiplication_struct *args = (struct matrix_multiplication_struct *)arguments;

/** matrix_multiplication_struct *******************************************
	int *array1;
	int *array2;
	int *array3;

	int row_index; //row index of array1, column index of array2
***************************************************************************/
 	printf("In dot_product function, row_index = %d\n", args->row_index);
	int sum = 0;	
	for(int j=0; j<column_B_size; j++)
	{
		/** k = column index for first matrix and row index for second matrix **/
		for(int k=0; k<column_A_size; k++)
		{
			sum += (*args->array1)[(args->row_index)][k] * (*args->array2)[k][j];
		}
	
		(*args->array3)[args->row_index][j] = sum;		
		sum = 0;
	}
	err1 = pthread_mutex_unlock(lock);
	if(err1 !=0)
		printf("Error unlocking mutex, err=%s.\n", strerror(err1));

	pthread_exit((void*) 0);
	return 0;
}
	
	/********************************************************************************/
	/** Print contents of matrix_B **************************************************/
	printf("matrix_B:\n");
	for(int i=0; i<row_B_size; i++)
	{
		for(int j=0; j<column_B_size; j++)
		{
			printf("%d ", array_B[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	/********************************************************************************/
	/** Create struct to pass multiple arguments to pthread_create() ***************/
	struct matrix_multiplication_struct arguments;

	arguments.array1 = &array_A;
	arguments.array2 = &array_B;
	arguments.array3 = &array_C;
	/*******************************************************************************/
	/** Test arguments->arrayx *****************************************************/
	printf("matrix_multiplication_struct arguments->array1:\n");
	for(int i=0; i<row_A_size; i++)
	{
		for(int j=0; j<column_A_size; j++)
		{
			printf("%d ", (*arguments.array1)[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	printf("matrix_multiplication_struct arguments->array2:\n");
	for(int i=0; i<row_B_size; i++)
	{
		for(int j=0; j<column_B_size; j++)
		{
			printf("%d ", (*arguments.array2)[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	/********************************************************************************/
	/** declare and initilize thread lock *******************************************/

	lock = malloc(sizeof(pthread_mutex_t));
	err=pthread_mutex_init(lock, NULL);
	if(err!=0)
		printf("Could not create mutex, err=%s.\n", strerror(err));
	/********************************************************************************/
	/** Create threads, join them with main thread  and lock them *******************/
	printf("creating, joining, and locking threads.................\n");
	printf("\n");
	/** thread 1 ********************************************************/
	err1 = pthread_mutex_lock(lock);
	if(err1 !=0)
		printf("Error locking mutex, err=%s.\n", strerror(err1));

	arguments.row_index = 0; 
	err=pthread_create(&thread_1, NULL, dot_product, (void *)&arguments);
	if(err !=0)
		printf("Error creating thread: %d\n", err);

	err2=pthread_join(thread_1, &tret1);
	if(err2 !=0)
		printf("Error joining thread: %d\n", err);

	err1 = pthread_mutex_unlock(lock);
	if(err1 !=0)
		printf("Error unlocking mutex, err=%s.\n", strerror(err1));

	/** thread 2 ********************************************************/
	err1 = pthread_mutex_lock(lock);
	if(err1 !=0)
		printf("Error locking mutex, err=%s.\n", strerror(err1));

	arguments.row_index = 1; 
	err = pthread_create(&thread_2, NULL, dot_product, (void *)&arguments);
	if(err !=0)
		printf("Error creating thread: %d\n", err);

	err2 = pthread_join(thread_2, &tret2);
	if(err2 !=0)
		printf("Error joining thread: %d\n", err);

	err1 = pthread_mutex_unlock(lock);
	if(err1 !=0)
		printf("Error unlocking mutex, err=%s.\n", strerror(err1));

	/** thread 3 ********************************************************/
	err1 = pthread_mutex_lock(lock);
	if(err1 !=0)
		printf("Error locking mutex, err=%s.\n", strerror(err1));

	arguments.row_index = 2; 
	err = pthread_create(&thread_3, NULL, dot_product, (void *)&arguments);
	if(err !=0)
		printf("Error creating thread: %d\n", err);

	err2 = pthread_join(thread_3, &tret3);
	if(err2 !=0)
		printf("Error joining thread: %d\n", err);

	err1 = pthread_mutex_unlock(lock);
	if(err1 !=0)
		printf("Error unlocking mutex, err=%s.\n", strerror(err1));

	/********************************************************************************/
	/** print exit codes of each thread *********************************************/

	//printf("Thread %d has exit code %ld.\n", (int)thread_1, (long)tret1);
	//printf("Thread %d has exit code %ld.\n", (int)thread_2, (long)tret2);
	//printf("Thread %d has exit code %ld.\n", (int)thread_3, (long)tret3);

	//printf("\n");
	/********************************************************************************/
	/** Print contents of matrix_C **************************************************/
	printf("matrix_C:\n");
	for(int i=0; i<row_A_size; i++)
	{
		for(int j=0; j<column_B_size; j++)
		{
			printf("%d ", array_C[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	/********************************************************************************/
	return 0;
}

/** Function performs dot product multiplication or given row from array1***************************************/

void *dot_product(void *arguments)
{
	struct matrix_multiplication_struct *args = (struct matrix_multiplication_struct *)arguments;

/***********************************
	int *array1;
	int *array2;
	int *array3;

	int row_index; //row index of array1, column index of array2
************************************/
 	printf("In dot_product function, row_index = %d\n", args->row_index);
	int sum = 0;	
	for(int j=0; j<column_B_size; j++)
	{
		/** k = column index for first matrix and row index for second matrix **/
		for(int k=0; k<column_A_size; k++)
		{
			sum += (*args->array1)[(args->row_index)][k] * (*args->array2)[k][j];
		}
	
		(*args->array3)[args->row_index][j] = sum;		
		sum = 0;
	}

	pthread_exit((void*) 0);
	return 0;
}
