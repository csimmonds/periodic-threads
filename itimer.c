/* 
 * Copyright (C) 2009 Chris Simmonds (chris@2net.co.uk)
 *
 * This is a demonstration of periodic threads using setitimer.
 *
 * Using glibc/nptl signals are per process, so only one thread can handle
 * the SIGALRM so in this example, the other thread will see at most one event
 *
 * Using uClibc/LinuxThreads, each thread has its own signal handler so each
 * thread sees the number of events expected
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

struct periodic_info {
	sigset_t alarm_sig;
};

static int make_periodic(unsigned int period, struct periodic_info *info)
{
	int ret;
	struct itimerval value;

	/* Block SIGALRM in this thread */
	sigemptyset(&(info->alarm_sig));
	sigaddset(&(info->alarm_sig), SIGALRM);
	pthread_sigmask(SIG_BLOCK, &(info->alarm_sig), NULL);

	/* Set the timer to go off after the first period and then
	   repetitively */
	value.it_value.tv_sec = period / 1000000;
	value.it_value.tv_usec = period % 1000000;
	value.it_interval.tv_sec = period / 1000000;
	value.it_interval.tv_usec = period % 1000000;
	ret = setitimer(ITIMER_REAL, &value, NULL);
	if (ret != 0)
		perror("Failed to set timer");
	return ret;
}

static void wait_period(struct periodic_info *info)
{
	int sig;

	/* Wait for the next SIGALRM */
	sigwait(&(info->alarm_sig), &sig);
}

static int thread_1_count;
static int thread_2_count;

static void *thread_1(void *arg)
{
	struct periodic_info info;

	printf("Thread 1 period 10ms\n");
	make_periodic(10000, &info);
	while (1) {
		thread_1_count++;
		wait_period(&info);
	}
	return NULL;
}

static void *thread_2(void *arg)
{
	struct periodic_info info;

	printf("Thread 2 period 20ms\n");
	make_periodic(20000, &info);
	while (1) {
		thread_2_count++;
		wait_period(&info);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t t_1;
	pthread_t t_2;
	sigset_t alarm_sig;

	printf("Periodic threads using setitimer\n");

	/* Block SIGALRM (not really necessary with uClibc) */
	sigemptyset(&alarm_sig);
	sigaddset(&alarm_sig, SIGALRM);
	sigprocmask(SIG_BLOCK, &alarm_sig, NULL);

	pthread_create(&t_1, NULL, thread_1, NULL);
	pthread_create(&t_2, NULL, thread_2, NULL);
	sleep(10);
	printf("Thread 1 %d iterations\n", thread_1_count);
	printf("Thread 2 %d iterations\n", thread_2_count);
	return 0;
}
