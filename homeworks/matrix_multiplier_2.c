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
#include <unistd.h>

#define number_of_A_rows 3 //number of rows
#define number_of_A_columns 4 //number of columns

#define number_of_B_rows 4 //number of rows
#define number_of_B_columns 3 //number of columns

void *dot_product(void *arguments);

pthread_mutex_t *lock;

const int number_of_threads = number_of_A_rows * number_of_B_columns;
int row_index=0;
int column_index=0;
int *sum;
void *void_ptr_return;
/********************************************************************/
struct matrix_multiplication_struct
{
	int (*array1)[number_of_A_rows][number_of_A_columns];
	int (*array2)[number_of_B_rows][number_of_B_columns];
};
/********************************************************************/

int main(void)
{
	int array_A[number_of_A_rows][number_of_A_columns] = { {1,2,3,4},{5,6,7,8},{9,10,11,12} };
	int array_B[number_of_B_rows][number_of_B_columns] = { {2,3,5},{7,11,13},{17,19,23},{29,31,37} };
	int array_C[number_of_A_rows][number_of_B_columns];

	int err, err1, err2, sum;
	/** create array of threads **/
	pthread_t threads[number_of_threads]; //create a thread for each dot product of the row

	/*********************************************************************************/
	/** Print contents of matix_A ****************************************************/
	printf("matrix_A:\n");
	for(int i=0; i<number_of_A_rows; i++)
	{
		for(int j=0; j<number_of_A_columns; j++)
		{
			printf("%d ", array_A[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	
	/********************************************************************************/
	/** Print contents of matrix_B **************************************************/
	printf("matrix_B:\n");
	for(int i=0; i<number_of_B_rows; i++)
	{
		for(int j=0; j<number_of_B_columns; j++)
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
	/*******************************************************************************/
	/** Test arguments->arrayx *****************************************************/

	/*
	printf("matrix_multiplication_struct arguments->array1:\n");
	for(int i=0; i<number_of_A_rows; i++)
	{
		for(int j=0; j<number_of_A_columns; j++)
		{
			printf("%d ", (*arguments.array1)[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	printf("matrix_multiplication_struct arguments->array2:\n");
	for(int i=0; i<number_of_B_rows; i++)
	{
		for(int j=0; j<number_of_B_columns; j++)
		{
			printf("%d ", (*arguments.array2)[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	*/
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

	/** Create a thread for each column of the second matrix to be multiplied by the row of the first **/
	for(int i=0; i<number_of_A_rows; i++)
	{
		for(int j=0; j<number_of_B_columns; j++)
		{
			err = pthread_create(&threads[(i*number_of_B_columns)+j], NULL, dot_product, (void *)&arguments);
			if(err !=0)
				printf("Error creating thread: %lu, error: %d\n", threads[(i*number_of_B_columns)+j], err);
		}
	}
	/**************************************************************************************************/
	/*
	for(int i=0; i<number_of_threads; i++)
		printf("thread %d id: %lu\n", i, threads[i]);
	*/

	/** Joining each thread to the main thread ********************************************************/

	int *temp_array;
	int A, B;
	for(int i=0; i<number_of_A_rows; i++)
	{	
		for(int j=0; j<number_of_B_columns; j++)
		{
			err=pthread_join(threads[(i*number_of_B_columns)+j], &void_ptr_return);
			if(err != 0)
				printf("Error joining thread: %d\n", err);

			//array_C[i][j] = *((int*)void_ptr_return);
			temp_array = ((int*)void_ptr_return);
			
			A=temp_array[1];
			B=temp_array[2];
			array_C[A][B] = temp_array[0];

			free(void_ptr_return);
		}
	}
	
	/********************************************************************************/
	/** Print contents of matrix_C **************************************************/
	printf("matrix_C:\n");
	for(int i=0; i<number_of_A_rows; i++)
	{
		for(int j=0; j<number_of_B_columns; j++)
		{
			printf("%d ", array_C[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	err=pthread_mutex_destroy(lock);
	/********************************************************************************/
	return 0;
}

/** Function performs dot product multiplication or given row from array1***************************************/

void *dot_product(void *arguments)
{
	/** Thread lock to prevent arguments.row_index from being altered before function finishes executing **/
	int err1 = pthread_mutex_lock(lock);
	if(err1 !=0)
		printf("Error locking mutex, err=%s.\n", strerror(err1));
	/*****************************************************************************************************/

	/*****************************************************************************************************/
	struct matrix_multiplication_struct *args = (struct matrix_multiplication_struct *)arguments;
	/****************************************************************************************************/

	/** If the column_index exceeds the number of columns it is time to move to the next row ***********/
	if(column_index == number_of_B_columns)
	{
		column_index = 0;
		row_index++;
	}
	/****************************************************************************************************/
	
	//printf("In dot_product function, row_index = %d column_index = %d\n", row_index, column_index);
	//printf("Thread ID: %lu\n", pthread_self() );
	//printf("-------------------------------------------------------------\n");
	
	int sum = 0;
	int *sum_array = (int*)malloc(3*sizeof(int));
	
	for(int j=0; j<number_of_A_columns; j++)
		sum += (*args->array1)[row_index][j] * (*args->array2)[j][column_index];

	sum_array[0] = sum;
	sum_array[1] = row_index;
	sum_array[2] = column_index;

	//printf("sum_array: %d %d %d\n", sum_array[0], sum_array[1], sum_array[2]);

	column_index++;
	//printf("column index: %d\n", column_index);

	/** unlock mutex ***********************************************************************************/
	err1 = pthread_mutex_unlock(lock);
	if(err1 !=0)
		printf("Error unlocking mutex, err=%s.\n", strerror(err1));
	/***************************************************************************************************/

	pthread_exit(sum_array);
	//pthread_exit(sum);	
	//pthread_exit((void*) 0);
}
