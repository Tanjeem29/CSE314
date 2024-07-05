#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  rw->reader_count = 0;
  rw->writer_count = 0;
  
  sem_init(&rw->writer_lock, 0 ,1);
  sem_init(&rw->reader_count_lock, 0 ,1); 

  sem_init(&rw->reader_lock, 0 ,1);
  sem_init(&rw->writer_count_lock, 0 ,1); 
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  sem_wait(&rw->reader_lock);
  sem_wait(&rw->reader_count_lock);
  rw->reader_count ++;
  if(rw->reader_count == 1){
    sem_wait(&rw->writer_lock);
  }
  sem_post(&rw->reader_count_lock);
  sem_post(&rw->reader_lock);
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  sem_wait(&rw->reader_count_lock);
  rw->reader_count--;
  if(rw->reader_count == 0){
    sem_post(&rw->writer_lock);
  }
  sem_post(&rw->reader_count_lock);
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  sem_wait(&rw->writer_count_lock);
  rw->writer_count++;
  if(rw->writer_count == 1){
    sem_wait(&rw->reader_lock);
  }
  sem_post(&rw->writer_count_lock);
  sem_wait(&rw->writer_lock);
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer
  sem_post(&rw->writer_lock);
  sem_wait(&rw->writer_count_lock);
  rw->writer_count--;
  if(rw->writer_count == 0){
    sem_post(&rw->reader_lock);
  }
  sem_post(&rw->writer_count_lock);
  
}