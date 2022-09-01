#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define THREADS 6

struct range
{
  /* start of the array for each thread */
  int start;
  /* size of the entire array */
  int size;
  /* array */
  int* data;
};

/* used for qsort */
int compfunc(const void *p1, const void *p2)
{
  return (*(int*)p1 - *(int*)p2);
}

int findMedian(int* arr, int size)
{
  int mid, mid2;
  if(size%2 == 0)
  {
    mid = size/2+0.5;
    mid2 = size/2-0.5;
    return (arr[mid] + arr[mid2])/2;
  }
  else
  {
    mid = size/2;
    return arr[mid];
  }
}

void* sortArrays(void* parameters)
{
  struct range* r = (struct range*) parameters;
  /* quicksort function given in stdlib.h */
  qsort(r->data + r->start, r->size, sizeof(int), compfunc);
  return NULL;
}

int main(int argc, char* argv[])
{

  char* inputFile;
  char path_name[100];
  FILE* input;
  int checkr;
  int size;

  pthread_t thread1_id;
  pthread_t thread2_id;
  pthread_t thread3_id;
  pthread_t thread4_id;
  pthread_t thread5_id;
  pthread_t thread6_id;

  struct range thread1_args;
  struct range thread2_args;
  struct range thread3_args;
  struct range thread4_args;
  struct range thread5_args;
  struct range thread6_args;

  if(argc == 2)
  {
    //store input file name
    sprintf(path_name, "%s", argv[1]);
    inputFile = path_name;
  }
  else
  {
    //print command line template
    printf("Command line input should be: [executable] [input file name]\n");
    exit(-1);
  }

  //open file and check if successful
  input = fopen(inputFile, "r");
  if(input == NULL)
  {
    fprintf(stderr, "File %s could not be opened\n", inputFile);
    exit(1);
  }

  /* get the size of the file from the first line */
  char buffer;
  fscanf(input, "%c %d", &buffer, &size);

  /* create and allocate the array */
  int* arr = malloc(size * sizeof(int));

  /* read in the rest of the file into the int array */
  int i = 0;
  while(i < size)
  {
    fscanf(input, "%d", &arr[i]);
    i++;
  }

  int remain = size%THREADS;
  int block = (size - (size%THREADS))/THREADS;

  thread1_args.size = block;
  thread1_args.data = arr;
  thread1_args.start = 0;
  pthread_create(&thread1_id, NULL, &sortArrays, &thread1_args);

  thread2_args.size = block;
  thread2_args.data = arr;
  thread2_args.start = block;
  pthread_create(&thread2_id, NULL, &sortArrays, &thread2_args);

  thread3_args.size = block;
  thread3_args.data = arr;
  thread3_args.start = block*2;
  pthread_create(&thread3_id, NULL, &sortArrays, &thread3_args);

  thread4_args.size = block;
  thread4_args.data = arr;
  thread4_args.start = block*3;
  pthread_create(&thread4_id, NULL, &sortArrays, &thread4_args);

  thread5_args.size = block;
  thread5_args.data = arr;
  thread5_args.start = block*4;
  pthread_create(&thread5_id, NULL, &sortArrays, &thread5_args);

  thread6_args.size = block+remain;
  thread6_args.data = arr;
  thread6_args.start = block*5;
  pthread_create(&thread6_id, NULL, &sortArrays, &thread6_args);

  pthread_join(thread1_id, NULL);
  pthread_join(thread2_id, NULL);
  pthread_join(thread3_id, NULL);
  pthread_join(thread4_id, NULL);
  pthread_join(thread5_id, NULL);
  pthread_join(thread6_id, NULL);

  int small = 0;
  //indices of each thread array
  int t1 = 0;
  int t2 = block;
  int t3 = block*2;
  int t4 = block*3;
  int t5 = block*4;
  int t6 = block*5;

  int sorted[size];
  /* final merge sort */
  for(int j = 0; j < size; j++)
  {
    bool c1 = false;
    bool c2 = false;
    bool c3 = false;
    bool c4 = false;
    bool c5 = false;
    bool c6 = false;
    if(t1 < block)
    {
      if(thread1_args.data[t1] <= thread2_args.data[t2])
      {
        small = thread1_args.data[t1];
        c1 = true;
      }
      else
      {
        small = thread2_args.data[t2];
        c2 = true;
        c1 = false;
      }
    }
    else if(t2 < block*2)
    {
      small = thread2_args.data[t2];
      c2 = true;
    }
    if(thread3_args.data[t3] <= small && t3 < block*3)
    {
      small = thread3_args.data[t3];
      c3 = true;
      c1 = false;
      c2 = false;
    }
    if(thread4_args.data[t4] <= small && t4 < block*4)
    {
      small = thread4_args.data[t4];
      c4 = true;
      c1 = false;
      c2 = false;
      c3 = false;
    }
    if(thread5_args.data[t5] <= small && t5 < block*5)
    {
      small = thread5_args.data[t5];
      c5 = true;
      c1 = false;
      c2 = false;
      c3 = false;
      c4 = false;
    }
    if(thread6_args.data[t6] <= small && t6 < block*6)
    {
      small = thread6_args.data[t6];
      c6 = true;
      c1 = false;
      c2 = false;
      c3 = false;
      c4 = false;
      c5 = false;
    }
    /* check which value was the smallest and increment that thread index*/
    if(c1 == true)
      t1++;
    if(c2 == true)
      t2++;
    if(c3 == true)
      t3++;
    if(c4 == true)
      t4++;
    if(c5 == true)
      t5++;
    if(c6 == true)
      t6++;

    //reorder initial array using the sorted threaded arrays
    sorted[j] = small;
  }

  /* finding mode after merge sort */
  int mode;
  int count = 0;
  for(int n = 0; n < size; n++)
  {
    if(sorted[n] == sorted[n+1])
    {
      int x = 1;
      while(sorted[n] == sorted[n + 1])
      {
        n++;
        x++;
        if(x > count)
        {
          count = x;
          mode = sorted[n];
        }
      }
    }
  }

    printf("Median = %d\n", findMedian(sorted, size));
    printf("Mode is %d and is listed %d times\n", mode, count);
  return 0;
}
