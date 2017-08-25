#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class sem{
public:
	/* create and initialize semaphore */
	sem(){
		if(sem_init(&m_sem, 0, 0) != 0)
			throw std::exception();
	}

	/* destory the semaphore */
	~sem(){
		sem_destroy(&m_sem);
	}

	/* wait for the semaphore */
	bool wait(){
		return (sem_wait(&m_sem) == 0);
	}

	/* post the semaphore */
	bool post(){
		return (sem_post(&m_sem) == 0);
	}

private:
	sem_t m_sem;
};

/* mutex wrapper */
class mutex{
public:
	/* create and initialize the mutex */
	mutex(){
		if(pthread_mutex_init(&m_mutex, NULL) != 0)
			throw std::exception();
	}

	/* destory the mutex */
	~mutex(){
		pthread_mutex_destroy(&m_mutex);
	}

	/* lock */
	bool lock(){
		return (pthread_mutex_lock(&m_mutex) == 0);
	}

	/* unlock */
	bool unlock(){
		return (pthread_mutex_unlock(&m_mutex) == 0);
	}

private:
	pthread_mutex_t m_mutex;
};

/* condition variable */
class condition_variable{
public:
	/* construct condition variable and initialize it */
	condition_variable(){
		if(pthread_mutex_init(&m_mutex, NULL) != 0)
			throw std::exception();
		if(pthread_cond_init(&m_cond, NULL) != 0){
			pthread_mutex_destroy(&m_mutex);
			throw std::exception();
		}
	}

	/* destory condition variable */
	~condition_variable(){
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_cond);
	}

	/* wait for a condition variable */
	bool wait(){
		int ret = 0;
		pthread_mutex_lock(&m_mutex);
		ret = pthread_cond_wait(&m_cond, &m_mutex);
		pthread_mutex_unlock(&m_mutex);
		return (ret == 0);
	}

	/* wake up the thread waits for the condtion variable */
	bool signal(){
		return (pthread_cond_signal(&m_cond) == 0);
	}

private:
	pthread_mutex_t m_mutex;
	pthread_cond_t  m_cond;
};

#endif
