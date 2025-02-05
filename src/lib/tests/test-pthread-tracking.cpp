/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tests/test-pthread-tracking.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

/**********************************************************/
#include <pthread.h>
#include <signal.h>
#include <cstdio>
#include <unistd.h>
#include <wrapper/ThreadTracker.hpp>

/**********************************************************/
#define NB_THREADS 16

/**********************************************************/
pthread_mutex_t mutex;

/**********************************************************/
void * thread_main_1(void *)
{
	fprintf(stderr,"New thread and simple exit\n");
	return NULL;
}

/**********************************************************/
void * thread_main_2(void *)
{
	fprintf(stderr,"New thread and pthread_exit\n");
	pthread_exit(NULL);
}

/**********************************************************/
void * thread_main_3(void *)
{
	fprintf(stderr,"New thread and lock\n");
	pthread_mutex_lock(&mutex);
	pthread_mutex_unlock(&mutex);
	return NULL;
}

/**********************************************************/
int main(void)
{
	pthread_t t[1024];
	int cnt=0;
	int cntJoin = 0;
	void * ret;
	
	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_lock(&mutex);

	//////////////////////////////////////////////////
	fprintf(stderr,"----- use thread_main_1 ----\n");
	for (int i = 0 ; i < NB_THREADS ; i++)
		pthread_create(&t[cnt++],NULL,thread_main_1,NULL);
	
	for (int i = 0 ; i < NB_THREADS ; i++)
		pthread_join(t[cntJoin++],&ret);
	
	fprintf(stderr,"Max threads : %d, current : %d\n",MALT::ThreadTracker::getMaxThreadCount(),MALT::ThreadTracker::getThreadCount());
	//////////////////////////////////////////////////
	
	fprintf(stderr,"---- use thread_main_2 ----\n");
	for (int i = 0 ; i < NB_THREADS ; i++)
		pthread_create(&t[cnt++],NULL,thread_main_2,NULL);
	
	for (int i = 0 ; i < NB_THREADS ; i++)
		pthread_join(t[cntJoin++],&ret);
	
	fprintf(stderr,"Max threads : %d, current : %d\n",MALT::ThreadTracker::getMaxThreadCount(),MALT::ThreadTracker::getThreadCount());
	//////////////////////////////////////////////////
	
	fprintf(stderr,"---- use thread_main_3 ----\n");
	for (int i = 0 ; i < NB_THREADS ; i++)
		pthread_create(&t[cnt++],NULL,thread_main_3,NULL);

	fprintf(stderr,"Max threads : %d, current : %d\n",MALT::ThreadTracker::getMaxThreadCount(),MALT::ThreadTracker::getThreadCount());
	sleep(1);
	fprintf(stderr,"Max threads : %d, current : %d\n",MALT::ThreadTracker::getMaxThreadCount(),MALT::ThreadTracker::getThreadCount());
	
	pthread_mutex_unlock(&mutex);
	
	//wait
	for (int i = 0 ; i < NB_THREADS ; i++)
		pthread_join(t[cntJoin++],&ret);
	
	fprintf(stderr,"------------------------------------\n");
	fprintf(stderr,"Max threads : %d, current : %d\n",MALT::ThreadTracker::getMaxThreadCount(),MALT::ThreadTracker::getThreadCount());
	
	return 0;
}
