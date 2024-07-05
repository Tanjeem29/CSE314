#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
// #include <semaphore.h>

int item_to_produce = 0, curr_buf_size = 0, num_produced = 0;
int item_to_consume = 0, curr_buf_size2 = 0, num_consumed = 0, num_in_buff = 0;
int total_items, max_buf_size, num_workers, num_masters;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock;
// sem_t emptybuffer;
// sem_t fullbuffer;
// sem_t buffermutex;

int *buffer;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}


void init_buff(){
  for(int i=0;i<max_buf_size;i++){
    buffer[i] = -1;
  }
}
void do_fill(){
  //int i= 0;
  while(buffer[curr_buf_size] != -1){
    curr_buf_size++;
    curr_buf_size%=max_buf_size;
  }
  buffer[curr_buf_size] = item_to_produce;
    //printf("at %d\n", curr_buf_size);

  num_in_buff++;
  num_produced++;
}
void do_get(){
  //int i= 0;
  while(buffer[curr_buf_size2] == -1){
    curr_buf_size2++;
    curr_buf_size2%=max_buf_size;
  }
  item_to_consume = buffer[curr_buf_size2];
  buffer[curr_buf_size2] = -1;
  //printf("at %d\n", curr_buf_size2);
  num_in_buff--;
  num_consumed++;
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
    {
      pthread_mutex_lock(&lock);

      //---------------------------
      if(num_produced >= total_items) {
        //printf("DONE MASTER %d %d\n", thread_id, num_produced);
        pthread_cond_broadcast(&fill);
        pthread_mutex_unlock(&lock);
	      break;
      }
      //---------------------------



      while (num_in_buff  == max_buf_size)
      {
        //printf("*******full******* %d\n", thread_id);
        pthread_cond_wait(&empty, &lock);
          if(num_produced >= total_items) {
          //printf("DONE MASTER %d %d\n", thread_id, num_produced);
          pthread_cond_broadcast(&fill);
          pthread_mutex_unlock(&lock);
          return 0;
          //break;
        }
      }

      //---------------------------
      // if(num_produced >= total_items) {
      //   printf("DONE MASTER.. %d %d\n", thread_id, num_produced);
      //   pthread_cond_broadcast(&fill);
      //   pthread_mutex_unlock(&lock);
	    //   break;
      // }
      //---------------------------


      do_fill();
      print_produced(item_to_produce, thread_id);
      item_to_produce++;
      pthread_cond_signal(&fill);

            //---------------------------
      // if(num_produced >= total_items) {
      //   printf("DONE MASTER.... %d %d\n", thread_id, num_produced);
      //   pthread_cond_broadcast(&fill);
      //   pthread_mutex_unlock(&lock);
	    //   break;
      // }
      //---------------------------

      pthread_mutex_unlock(&lock);
      //printf()
    }
  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item
void *consume_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
    {
      pthread_mutex_lock(&lock);
     // printf("test\n");
     //---------------------------
      if(num_consumed >= total_items) {
	      //printf("DONE WORKER %d %d\n", thread_id, num_consumed);
        pthread_mutex_unlock(&lock);
        break;
      }
      //---------------------------


      while (num_in_buff == 0)
      {
        //printf("*******empty******* %d\n", thread_id);
        pthread_cond_wait(&fill, &lock);
          if(num_consumed >= total_items) {
          //printf("DONE WORKER... %d %d\n", thread_id, num_consumed);
          pthread_mutex_unlock(&lock);
          return 0;
          //break;
        }
      }


      //---------------------------
      // if(num_consumed >= total_items) {
	    //   printf("DONE WORKER.. %d %d\n", thread_id, num_consumed);
      //   pthread_mutex_unlock(&lock);
      //   break;
      // }
      //---------------------------


      do_get();
      print_consumed(item_to_consume, thread_id);
      pthread_cond_signal(&empty);


//---------------------------
      // if(num_consumed >= total_items) {
	    //   printf("DONE WORKER.... %d %d\n", thread_id, num_consumed);
      //   pthread_mutex_unlock(&lock);
      //   break;
      // }
//---------------------------


      pthread_mutex_unlock(&lock);
    }
  return 0;
}

int main(int argc, char *argv[])
{
  int *master_thread_id;
  pthread_t *master_thread;
  item_to_produce = 0;
  curr_buf_size = 0;

  int *worker_thread_id;
  pthread_t *worker_thread;
  item_to_consume = 0;
  curr_buf_size2 = 0;
  

  int i;
  
   if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }
    
  // sem_init(&buffermutex,0,1);
  // sem_init(&emptybuffer,0,max_buf_size);
  // sem_init(&fullbuffer,0,0);



   buffer = (int *)malloc (sizeof(int) * max_buf_size);
  init_buff();
   //create master producer threads
   master_thread_id = (int *)malloc(sizeof(int) * num_masters);
   master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

  for (i = 0; i < num_masters; i++){
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
    printf("master %d created\n", i);
    // pthread_join(master_thread[i], NULL);
    // printf("master %d joined\n", i);

  }
  
  //create worker consumer threads
  worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
  worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  for (i = 0; i < num_workers; i++)
    worker_thread_id[i] = i;

  for (i = 0; i < num_workers; i++){
    pthread_create(&worker_thread[i], NULL, consume_requests_loop, (void *)&worker_thread_id[i]);
    printf("worker %d created\n", i);
    // pthread_join(worker_thread[i], NULL);
    // printf("worker %d joined\n", i);
  }
  //   pthread_create(&worker_thread[i], NULL, consume_requests_loop, (void *)&worker_thread_id[i]);
  // //wait for all master threads to complete
    for (i = 0; i < num_masters; i++)
    {
      pthread_join(master_thread[i], NULL);
      printf("master %d joined\n", i);
    }
  // //wait for all worker threads to complete
  for (i = 0; i < num_workers; i++)
    {
      pthread_join(worker_thread[i], NULL);
      printf("worker %d joined\n", i);
    }
  
  /*----Deallocating Buffers---------------------*/
  printf("******freeing all******\n");
  free(buffer);
  free(master_thread_id);
  free(master_thread);
  free(worker_thread_id);
  free(worker_thread);
  

  return 0;
}
