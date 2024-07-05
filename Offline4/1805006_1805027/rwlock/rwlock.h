#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

struct read_write_lock
{
    //pthread_mutex_t reader_count_lock;
    //pthread_mutex_t writer_lock;
    sem_t reader_count_lock;
    sem_t writer_lock;

    sem_t writer_count_lock;
    sem_t reader_lock;

    int reader_count;
    int writer_count;
};

void InitalizeReadWriteLock(struct read_write_lock * rw);
void ReaderLock(struct read_write_lock * rw);
void ReaderUnlock(struct read_write_lock * rw);
void WriterLock(struct read_write_lock * rw);
void WriterUnlock(struct read_write_lock * rw);
