#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "mpi.h"
#include "bitonic_sort.h"


const int up = 1;
const int down = 0;

double timeStart;
double timeEnd;

char* inputPath = "data/input";
char* outputPath = "data/output";

FILE* fileInp;
FILE* fileOut;

int processRank;
int numProc;
int * array;
int SIZE;

const int MASTER 0
const int OUTPUT_NUM 10

int greatestPowerOfTwoLessThan(int n)
{
  int k=1;
  while(k>0 && k<n)
    k=k<<1;
  return k>>1;
}

void rng(int* arr, int n) {
  int seed = 13515104;
  srand(seed);
  for(long i = 0; i < n; i++) {
    arr[i] = (int)rand();
  }
}

void swap(int *x, int *y)
{
  int temp = *y;
  *y = *x;
  *x = temp;
}

void compare(int i, int j, int dir)
{
  int t;
  if (dir == (array[i] > array[j]))
  {
    swap(&array[i], &array[j]);
  }
}

void bitonicMerge(int low, int c, int dir)
{
  int k, i;
  if (c > 1)
  {
    k = greatestPowerOfTwoLessThan(c);
    for (i = low;i < low+c-k ;i++)
      compare(i, i+k, dir);
    bitonicMerge(low, k, dir);
    bitonicMerge(low+k, c-k, dir);
  }
}

void recursiveBitonic(int low, int c, int dir)
{
  int k;
  if (c > 1)
  {
    k = c / 2;
    recursiveBitonic(low, k, !dir);
    recursiveBitonic(low + k, c-k, dir);
    bitonicMerge(low, c, dir);
  }
}

void sort_serial()
{
  recursiveBitonic(0, SIZE, up);
}

int ComparisonFunc(const void * a, const void * b)
{
    return ( * (int *)a - * (int *)b );
}

void CompareLow(int j)
{
    int i, min;

    int sendCounter = 0;
    int * bufferSend = malloc((SIZE + 1) * sizeof(int));
    MPI_Send(
        &array[SIZE - 1],
        1,
        MPI_INT,
        processRank ^ (1 << j),
        0,
        MPI_COMM_WORLD
    );

    int recvCounter;
    int * bufferRecieve = malloc((SIZE + 1) * sizeof(int));
    MPI_Recv(
        &min,
        1,
        MPI_INT,
        processRank ^ (1 << j),
        0,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
    );

    for (i = 0; i < SIZE; i++)
     {
        if (array[i] > min)
         {
            bufferSend[sendCounter + 1] = array[i];
            sendCounter++;
        }
        else
        {
            break;
        }
    }

    bufferSend[0] = sendCounter;

    MPI_Send(
        bufferSend,
        sendCounter,
        MPI_INT,
        processRank ^ (1 << j),
        0,
        MPI_COMM_WORLD
    );

    MPI_Recv(
        bufferRecieve,
        SIZE,
        MPI_INT,
        processRank ^ (1 << j),
        0,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
    );

    for (i = 1; i < bufferRecieve[0] + 1; i++)
    {
        if (array[SIZE - 1] < bufferRecieve[i])
        {
            array[SIZE - 1] = bufferRecieve[i];
        }
        else
        {
            break;
        }
    }

    qsort(array, SIZE, sizeof(int), ComparisonFunc);

    free(bufferSend);
    free(bufferRecieve);

    return;
}

void CompareHigh(int j)
{
    int i, max;

    int recvCounter;
    int * bufferRecieve = malloc((SIZE + 1) * sizeof(int));
    MPI_Recv(
        &max,
        1,
        MPI_INT,
        processRank ^ (1 << j),
        0,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
    );

    int sendCounter = 0;
    int * bufferSend = malloc((SIZE + 1) * sizeof(int));
    MPI_Send(
        &array[0],
        1,
        MPI_INT,
        processRank ^ (1 << j),
        0,
        MPI_COMM_WORLD
    );

    for (i = 0; i < SIZE; i++)
    {
        if (array[i] < max)
        {
            bufferSend[sendCounter + 1] = array[i];
            sendCounter++;
        }
        else
        {
            break;
        }
    }

    MPI_Recv(
        bufferRecieve,
        SIZE,
        MPI_INT,
        processRank ^ (1 << j),
        0,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
    );
    recvCounter = bufferRecieve[0];

    bufferSend[0] = sendCounter;
    MPI_Send(
        bufferSend,
        sendCounter,
        MPI_INT,
        processRank ^ (1 << j),
        0,
        MPI_COMM_WORLD
    );

    for (i = 1; i < recvCounter + 1; i++)
    {
        if (bufferRecieve[i] > array[0])
        {
            array[0] = bufferRecieve[i];
        }
        else
        {
            break;
        }
    }

    qsort(array, SIZE, sizeof(int), ComparisonFunc);

    free(bufferSend);
    free(bufferRecieve);

    return;
}

int main(int argc, char * argv[])
{
    int i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

    SIZE = atoi(argv[1]);
    array = (int *) malloc(SIZE * sizeof(int));

    rng(array, SIZE);
    fileInp = fopen(inputPath, "w");
    for (i = 0; i < SIZE; i++)
    {
      fprintf(fileInp, "%d\n", array[i]);
    }
    fclose(fileInp);
    int dimensions = (int)(log2(numProc));

    if (processRank == MASTER)
    {
        timeStart = MPI_Wtime();
        sort_serial();
        timeEnd = MPI_Wtime();

        printf("[SERIAL]\n");
        printf("Lama sorting (Sec): %f\n", timeEnd - timeStart);

        free(array);
        array = (int*) malloc(SIZE * sizeof(int));
        rng(array, SIZE);

        timeStart = MPI_Wtime();
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (i = 0; i < dimensions; i++)
    {
        for (j = i; j >= 0; j--)
        {
            if (((processRank >> (i + 1)) % 2 == 0 &&
                  (processRank >> j) % 2 == 0) || ((processRank >> (i + 1)) % 2
                  != 0 && (processRank >> j) % 2 != 0))
            {
                CompareLow(j);
            }
            else
            {
                CompareHigh(j);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (processRank == MASTER)
    {
        timeEnd = MPI_Wtime();

        printf("[PARALLEL]\n");
        printf("Lama sorting (Sec): %f\n", timeEnd - timeStart);
    }

    fileOut = fopen(outputPath, "w");

    for (i = 0; i < SIZE; i++)
    {
      fprintf(fileOut, "%d\n", array[i]);
    }
    fclose(fileOut);
    free(array);

    MPI_Finalize();
    return 0;
}
