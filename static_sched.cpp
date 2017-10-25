#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <ratio>
#include <chrono>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

int functionid;
float a;
float b;
int n;
float (*f)(float, int);
float sum = 0;
pthread_mutex_t mutex;
int intensity;
int nbthreads;
char* sync;


typedef struct thread_data_t
{
  int start;
  int finish;
  float sum;
  
} thread_data_t;

//intregrate function using iteration sync
void* iteration(void* arg)
{
    thread_data_t *data = (thread_data_t *)arg;
    
    for(int j = data->start; j < data->finish; j++)
    {
      pthread_mutex_lock(&mutex);
      sum += f(a + (j + 0.5) * (b - a) / n, intensity) * (b - a) / n;
      pthread_mutex_unlock(&mutex);
    }
    
    pthread_exit(NULL);
}

//intregrate function using thread sync
void* thread(void* arg)
{
  thread_data_t *data = (thread_data_t *)arg;
  
  for(int j = data->start; j < data->finish; j++)
    data->sum += f(a + (j + 0.5) * (b - a) / n, intensity) * (b - a) / n;
    
}

/////////////////////////////////////////////////////////////////////
  
int main (int argc, char* argv[]) {

  if (argc < 8) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
    return -1;
  }
  
  pthread_mutex_init(&mutex, NULL);
  
   functionid = atoi(argv[0]);
  a = atof(argv[1]); 
  b = atof(argv[2]); 
  n = atoi(argv[3]);
  intensity = atoi(argv[4]);
  nbthreads = atoi(argv[5]);
  sync = argv[6];
  
  switch(functionid)
  {
    case 1:
      f = f1;
      break;
    case 2:
      f = f2;
      break;
    case 3:
      f = f3;
      break;
    case 4:
      f = f4;
      break;
  }
  
  //pthreads array
  pthread_t threads [nbthreads];
  
  //thread data
  thread_data_t thr[nbthreads];

  //the timer for the program
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  int N_Of_itsPerThread = n / nbthreads;
 
  for(int j = 0; j < nbthreads; j++)
  {    
    thr[j].start = j * N_Of_itsPerThread;
    thr[j].finish = (j + 1) * N_Of_itsPerThread;
    thr[j].sum = 0;
    
    if(j == nbthreads - 1)
      thr[j].finish = n;
      
    if(sync == "iteration")  
      pthread_create(&threads[j], NULL, iteration, &thr[j]);
    else if(sync == "thread")
      pthread_create(&threads[j], NULL, thread, &thr[j]);
  }
  
  //get all the threads together
  for (int j = 0; j < nbthreads; j++)
    pthread_join(threads[j], NULL);
    
  if(sync == "thread")
  {
    sum = 0;
    
    for(int j = 0; j < nbthreads; j++)
      sum += thr[j].sum;
      
  }
  
  //time for the program
  std::chrono::time_point<std::chrono::system_clock> finish = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = finish-start;
  
  //the output
  std::cout << sum << std::endl;
  std::cerr << elapsed_seconds.count()  << std::endl;
  
  return 0;
}