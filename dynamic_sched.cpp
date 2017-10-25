#include <ctime>

#include <cmath>

#include <chrono>

#include <cstdlib>

#include <iostream>

#include <pthread.h>


#include <string.h>


#ifdef __cplusplus
extern "C" {

#endif

float 

f1(float x, int intensity);

float f2(float x, int intensity);

float f3(float x, int intensity);

float f4(float x, int intensity);


#ifdef __cplusplus
}
#endif



int 
functionid, n, intensity, nbthreads, granularity;

int numberTemp = 0;

float a, b;

float total = 0;

char* sync;

float (*function)(float, int);

pthread_mutex_t mut1;

pthread_mutex_t mut2;

void getNext(int& begin , int& end) 
{

    pthread_mutex_lock(&mut2);
    
    begin = numberTemp; //0
  
    if((numberTemp + granularity) >= n) 
    
        end = n;
        
    else
    
        end = numberTemp + granularity;
        
    numberTemp += granularity;
    
    pthread_mutex_unlock(&mut2);
}

bool done()
{ 
  if(numberTemp >= n)
  {
    return true;
  }
  else 
  {
    return false;
  }
  
}





struct variables
{
  int begin;
  
  int end;
  
  float totalnumberTempum;
  
};





void* execute_inner_loop(void* arg) {
  
  struct variables *variable = (struct variables *)arg;
  
  while(!done()) { 
    
    getNext(variable->begin, variable->end);
    
    if(strcmp(sync, "iteration") == 0)
    
      for(int i = variable->begin; i < variable->end; i++) {
        
        pthread_mutex_lock(&mut1);
        
        total += function(a + (i + 0.5) * ((b - a) / n), intensity) * (b - a) / n;
        
        pthread_mutex_unlock(&mut1);
        
      }
      
    else if(strcmp(sync, "thread") == 0)
    
      for(int i = variable->begin; i < variable->end; i++) {
        
        variable->totalnumberTempum += function(a + (i + 0.5) * ((b - a) / n), intensity) * (b - a) / n;
        
      }
      
    else if(strcmp(sync, "chunk") == 0) {
      
      float totalChunknumberTempum = 0;
      
      for(int i = variable->begin; i < variable->end; i++) {
        
        totalChunknumberTempum += function(a + (i + 0.5) * ((b - a) / n), intensity) * (b - a) / n;
        
      }
      
      pthread_mutex_lock(&mut1);
      
      total += totalChunknumberTempum;
      
      pthread_mutex_unlock(&mut1);
      
    }
  }
  
}







int main (int argc, char* argv[]) {
  
  if (argc < 9) {
    
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    
    return -1;
    
  }

  functionid = atoi(argv[1]);
  
  a = atof(argv[2]);
  
  b = atof(argv[3]);
  
  n = atoi(argv[4]);
  
  intensity = atoi(argv[5]);
  
  nbthreads = atoi(argv[6]);
  
  sync = argv[7];
  
  granularity = atoi(argv[8]);
  
  pthread_t threads [nbthreads];
  
  variables var[nbthreads];
  
  switch(functionid) {
    
    case 1:
    
      function = f1;
      
      break;
      
    case 2:
    
      function = f2;
      
      break;
      
    case 3:
    
      function = f3;
      
      break;
      
    case 4:
    
      function = f4;
      
      break;
  
    
  }
  
  for(int i = 0; i < nbthreads; i++) {
  
    var[i].totalnumberTempum = 0;
    
  }
  
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  for(int i = 0; i < nbthreads; i++) {
    
    pthread_create(&threads[i], NULL, execute_inner_loop, &var[i]);
    
  }
      
      
  //executing
  
  for (int i = 0; i < nbthreads; i++) {
    
    pthread_join(threads[i], NULL);
    
  }
    
  
  if(strcmp(sync, "thread") == 0) {
    
    total = 0;
    
    for(int i = 0; i < nbthreads; i++) {
      
      total += var[i].totalnumberTempum;
      
    }
    
  }
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  
  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cerr << elapsed_seconds.count()  << std::endl;
  
  std::cout << total << std::endl;

  return 0;
}