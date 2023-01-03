#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

long maxTry = 999; // number of operations per thread
int maxThreads = 10; // number of threads
volatile long result = 0; // variables to be modified using threads
long t0cnt = 0; // number of executions of the "decrease" thread
long t1cnt = 0; // number of executions of the "increase" thread

// decrease result by 0 ~ maxTry
void* decrease(void* arg);
// increase result by 0 ~ maxTry
void* increase(void* arg);
// allowing threads to run long enough to cause synchronization problems
long swap(long x);

pthread_mutex_t mutex; // use mutex to make critical section

int main(int argc, char* argv[])
{
	pthread_t dec[100], inc[100];
	int retval;
	int i;

	if (argc <= 1) {
		printf("Usage: unsynced [init.result] [no.of.try] [no.of.threads]\n");
		exit(1);
	}
	if (argc >= 2) {
		result = atoi(argv[1]);
	}
	if (argc >= 3) {
		maxTry = atoi(argv[2]);
	}
	if (argc >= 4) {
		maxThreads = atoi(argv[3]);
	}
	if (argc > 4) {
		printf("Usage: unsynced [init.result] [no.of.try] [no.of.threads]\n");
		exit(1);
	}
	printf("Start Result -> %ld, MaxTry -> %ld, MaxThreads -> %d\n", result, maxTry, maxThreads);

	retval = pthread_mutex_init(&mutex, NULL);

	// create a sufficient number of threads so that synchronization problems can occur
	for (i = 0; i < maxThreads; i++) {
		retval = pthread_create(&dec[i], NULL, decrease, NULL);
		retval = pthread_create(&inc[i], NULL, increase, NULL);
	}

	// join the threads
	for (i = 0; i < maxThreads; i++) {
		retval = pthread_join(dec[i], NULL);
		retval = pthread_join(inc[i], NULL);
	}

	retval = pthread_mutex_destroy(&mutex);

	printf("Final Result -> %ld\n", result);

	return 0;
} /* end of main */

void* decrease(void* arg)
{
	long i;

	for (i = 0; i < maxTry; i++) {
		pthread_mutex_lock(&mutex); // start critical section
		result -= i;
		// allowing threads to run long enough to cause synchronization problems
		result = swap(result);
		pthread_mutex_unlock(&mutex); // end critical section
	}
	printf("decrease thread : maxTry -> %ld, decrease thread execution cnt -> %ld\n", maxTry, t0cnt++);

	pthread_exit(0);
}

void* increase(void* arg)
{
	long i;

	for (i = 0; i < maxTry; i++) {
		pthread_mutex_lock(&mutex); // start critical section
		result += i;
		// allowing threads to run long enough to cause synchronization problems
		result = swap(result);
		pthread_mutex_unlock(&mutex); // end critical section
	}
	printf("increase thread : maxTry -> %ld, increase thread execution cnt -> %ld\n", maxTry, t1cnt++);

	pthread_exit(0);
}

long swap(long x)
{
	long tmp;
	int i;

	// add a loop, which is a non atomic operation, to a thread
	for (i = 0; i < 100; i++) {
		tmp = x;
		x = tmp;
	}
	return x;
}