/* 
 * Copyright (C) 2009 Chris Simmonds (chris@2net.co.uk)
 *
 * This is a demonstration of periodic threads using POSIX timers and signals.
 * Each periodic thread is allocated a signal between SIGRTMIN to SIGRTMAX: we
 * assume that there are no other uses for these signals.
 *
 * All RT signals must be blocked in all threads before calling make_periodic()
 */

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

struct periodic_info {
	int sig;
	sigset_t alarm_sig;
};

static int make_periodic(int unsigned period, struct periodic_info *info)
{
	static int next_sig;
	int ret;
	unsigned int ns;
	unsigned int sec;
	struct sigevent sigev;
	timer_t timer_id;
	struct itimerspec itval;

	/* Initialise next_sig first time through. We can't use static
	   initialisation because SIGRTMIN is a function call, not a constant */
	if (next_sig == 0)
		next_sig = SIGRTMIN;
	/* Check that we have not run out of signals */
	if (next_sig > SIGRTMAX)
		return -1;
	info->sig = next_sig;
	next_sig++;
	/* Create the signal mask that will be used in wait_period */
	sigemptyset(&(info->alarm_sig));
	sigaddset(&(info->alarm_sig), info->sig);

	/* Create a timer that will generate the signal we have chosen */
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = info->sig;
	sigev.sigev_value.sival_ptr = (void *)&timer_id;
	ret = timer_create(CLOCK_MONOTONIC, &sigev, &timer_id);
	if (ret == -1)
		return ret;

	/* Make the timer periodic */
	sec = period / 1000000;
	ns = (period - (sec * 1000000)) * 1000;
	itval.it_interval.tv_sec = sec;
	itval.it_interval.tv_nsec = ns;
	itval.it_value.tv_sec = sec;
	itval.it_value.tv_nsec = ns;
	ret = timer_settime(timer_id, 0, &itval, NULL);
	return ret;
}

static void wait_period(struct periodic_info *info)
{
	int sig;
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
	int i;

	printf("Periodic threads using POSIX timers\n");

	/* Block all real time signals so they can be used for the timers.
	   Note: this has to be done in main() before any threads are created
	   so they all inherit the same mask. Doing it later is subject to
	   race conditions */
	sigemptyset(&alarm_sig);
	for (i = SIGRTMIN; i <= SIGRTMAX; i++)
		sigaddset(&alarm_sig, i);
	sigprocmask(SIG_BLOCK, &alarm_sig, NULL);

	pthread_create(&t_1, NULL, thread_1, NULL);
	pthread_create(&t_2, NULL, thread_2, NULL);
	sleep(10);
	printf("Thread 1 %d iterations\n", thread_1_count);
	printf("Thread 2 %d iterations\n", thread_2_count);
	return 0;
}
