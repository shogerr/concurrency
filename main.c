#include <assert.h>
#include <cpuid.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include "mt19937ar.h"

#define BUFFER_SIZE 32
enum THREADS { PRODUCER_THREAD, CONSUMER_THREAD, NUM_THREADS };

typedef struct
{
    int id;
    double t;
} BufferObject;

typedef struct
{
    BufferObject objects[BUFFER_SIZE];
    double (*gen)();
    int count;
    pthread_cond_t consumerCond;
    pthread_cond_t producerCond;
} Buffer;

pthread_mutex_t buffMutex;
Buffer gBuffer;

void buffer_add (Buffer *b)
{
    printf("buffer count!: %d\n", b->count);
    if (b->count < BUFFER_SIZE)
    {
        b->objects[b->count].id = b->gen()*1000;
        b->objects[b->count].t = b->gen()*7+2;
        b->count++;
    }
}

unsigned int supportRdrnd() {
    const unsigned int f = (1 << 30);

    unsigned int eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);

    return ((ecx & f) == f);
}

double rnd_mt19937()
{
    double x;
    x = genrand_real2();
    return x;
}

double rnd_rdrand()
{
   unsigned int x;
   __builtin_ia32_rdrand32_step(&x);
   //TODO Use result to create double between (0,1)
   return (double) x;
}

void* producer()
{
    for (;;)
    {
        pthread_mutex_lock(&buffMutex);

        int t = (int)(gBuffer.gen()*6+3);
        printf("time is: %d\n", t);
        //sleep(t);
        
        buffer_add(&gBuffer);

        pthread_mutex_unlock(&buffMutex);
        printf("%d\n", gBuffer.count);
    }

    return NULL;
}

void* consumer()
{
    for (;;)
    {
        sleep()
    }
    return NULL;
}

int main()
{
    int i;
    //
    // threads
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    void* status;
    int thread_result;

    gBuffer.count = 0;

    // random
    //double (*gen)();

    if (supportRdrnd())
        gBuffer.gen = rnd_rdrand;
    else
    {
        init_genrand(time(NULL));
        gBuffer.gen = rnd_mt19937;
    }


    pthread_mutex_init(&buffMutex, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    thread_result = pthread_create(&threads[PRODUCER_THREAD], NULL, producer, NULL);
    assert(!thread_result);

    thread_result = pthread_create(&threads[CONSUMER_THREAD], NULL, consumer, NULL);
    assert(!thread_result);

    pthread_attr_destroy(&attr);

    for (i = 0; i < NUM_THREADS; ++i)
    {
        pthread_join(threads[i], &status);
    }

    //printf("buffer count: %d\n", gBuffer.count);
    //printf("buffer[%d] is %d\n", gBuffer.count, gBuffer.objects[gBuffer.count-1].id);

    pthread_mutex_destroy(&buffMutex);
    pthread_exit(NULL);

/*
    for (;;)
    {
        
    }
*/

    return 0;
}
