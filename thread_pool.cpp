#include <bits/stdc++.h>
#include <unistd.h>
#include "thread_pool.h"
using namespace std;


typedef struct work{

	void (*routine) (void*);   
	void * arg;                
	struct work* next;			

} workUtil;


typedef struct thread_pooled {
   
	int num_threads;	
	int qsize;			
	pthread_t *threads;	
	workUtil* qHead;		
	workUtil* qTail;		
	pthread_cond_t q_NonEmpty;	
	pthread_cond_t q_Empty;
	pthread_mutex_t qlock;		
	
	int shutdown;
	int dont_accept;
} threadpoolUtil;

void* do_work(threadpool p) {
	threadpoolUtil * pool = (threadpoolUtil *) p;
	workUtil* cur;	
	

	while(1) {
		
		pthread_mutex_lock(&(pool->qlock));  


		while( pool->qsize == 0) {	  
			if(pool->shutdown) {
				pthread_mutex_unlock(&(pool->qlock));
				pthread_exit(NULL);
			}

			pthread_cond_wait(&(pool->q_NonEmpty),&(pool->qlock));

			if(pool->shutdown) {
				pthread_mutex_unlock(&(pool->qlock));
				pthread_exit(NULL);
			}
		}

		cur = pool->qHead;	

		pool->qsize--;
		
		if(pool->qsize == 0) {
			pool->qHead = NULL;
			pool->qTail = NULL;
		}
		else {
			pool->qHead = cur->next;
		}

		if(pool->qsize == 0 && ! pool->shutdown) {
			pthread_cond_signal(&(pool->q_Empty));
		}
		pthread_mutex_unlock(&(pool->qlock));
		(cur->routine) (cur->arg);   
		free(cur);					
	}
}


threadpool create_threadpool(int num_threads_in_pool)
 {
  	if ((num_threads_in_pool <= 0) || (num_threads_in_pool > MAX_THREADS))
    	return NULL;

  	threadpoolUtil *pool = (threadpoolUtil *) malloc(sizeof(threadpoolUtil));
	if (pool == NULL) {
	    fprintf(stderr, "Out of Memory creating a new threadpool!\n");
	    return NULL;
	  }

  pool->threads = (pthread_t*) malloc (sizeof(pthread_t) * num_threads_in_pool);

  if(pool->threads ==NULL) {
    fprintf(stderr, "Out of Memory creating a new threadpool!\n");
    return NULL;	
  }

  pool->num_threads = num_threads_in_pool; 
  pool->qsize = 0;
  pool->qHead = NULL;
  pool->qTail = NULL;
  pool->shutdown = 0;
  pool->dont_accept = 0;

  pthread_mutex_init(&pool->qlock,NULL);
  pthread_cond_init(&(pool->q_Empty),NULL);
  pthread_cond_init(&(pool->q_NonEmpty),NULL);

 for (int i = 0;i < num_threads_in_pool;i++) {
 	
	  if(pthread_create(&(pool->threads[i]),NULL,do_work,pool)) {
	    	fprintf(stderr, "Thread initiation error!\n");	
		    return NULL;	
  
	  }
  }

  return (threadpool)pool;
}



void dispatch(threadpool from_me,dispatch_fn dispatch_to_here,void *arg)
{
  threadpoolUtil *pool = (threadpoolUtil *)from_me;

	
	workUtil* cur = (workUtil*) malloc(sizeof(workUtil));
	if(cur == NULL) {
		fprintf(stderr, "Out of memory creating a work struct!\n");
		return;	
	}


	cur->routine = dispatch_to_here;
	cur->arg = arg;
	cur->next = NULL;

	pthread_mutex_lock(&(pool->qlock));

	if(pool->dont_accept) { 
		free(cur); 
		return;
	}

	if(pool->qsize == 0) {
		pool->qHead = cur; 
		pool->qTail = cur;
		pthread_cond_signal(&(pool->q_NonEmpty));   
	}
	else {
		pool->qTail->next = cur;	
		pool->qTail = cur;
	}
	pool->qsize++;
	pthread_mutex_unlock(&(pool->qlock));  


void destroy_threadpool(threadpool destroyme)
{
	threadpoolUtil *pool = (threadpoolUtil *) destroyme;

	pthread_mutex_lock(&(pool->qlock));
	pool->dont_accept = 1;
	while(pool->qsize != 0) {
		pthread_cond_wait(&(pool->q_Empty),&(pool->qlock));  
	}
	pool->shutdown = 1; 
	pthread_cond_broadcast(&(pool->q_NonEmpty));  
	pthread_mutex_unlock(&(pool->qlock));
 
	for(int i=0;i < pool->num_threads;i++) {

		pthread_join(pool->threads[i],NULL);
	}

	

	pthread_mutex_destroy(&(pool->qlock));
	pthread_cond_destroy(&(pool->q_Empty));
	pthread_cond_destroy(&(pool->q_NonEmpty));
	free(pool->threads);
	return;
}