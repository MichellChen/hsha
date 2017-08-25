// filename: threadpool.hpp
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "synchro.hpp"

template<typename T>
class threadpool{
public:
	threadpool(int numThreads = 8, int max_requests = 1000);
	~threadpool();
	/* 向请求队列添加任务 */
	bool append(T* request);

private:
	static void* worker(void* arg);
	void run();

private:
	int				m_numThreads;
	int				m_max_requests;
	pthread_t*		m_threads;
	std::list<T*>	m_workqueue;
	mutex			m_queuelocker;
	sem				m_queuestat;
	bool			m_stop;
};

template<typename T>
threadpool<T>::threadpool(int numThreads, int max_requests) :
		m_numThreads(numThreads), m_max_requests(max_requests), m_stop(false), m_threads(NULL){
	if(numThreads <= 0 || max_requests <=0)
		throw std::exception();

	m_threads = new pthread_t[m_numThreads];
	if(!m_threads)
		throw std::exception();
	/* 创建m_numThreads个线程，并将它们都设置为脱离线程 */
	for(int i = 0; i < m_numThreads; ++i){
		printf("create the %dth thread\n", i);
		if(pthread_create(m_threads + i, NULL, worker, this) != 0){
			delete[] m_threads;
			throw std::exception();
		}
		if(pthread_detach(m_threads[i])){
			delete[] m_threads;
			throw std::exception();
		}
	}
}

template<typename T>
threadpool<T>::~threadpool(){
	delete[] m_threads;
	m_stop = true;
}

template<typename T>
bool threadpool<T>::append(T* request){
	m_queuelocker.lock();
	if(m_workqueue.size() > m_max_requests){
		m_queuelocker.unlock();
		return false;
	}
	m_workqueue.push_back(request);
	m_queuelocker.unlock();
	m_queuestat.post();
	return true;
}

template<typename T>
void* threadpool<T>::worker(void* arg){
	threadpool* pool = (threadpool*)arg;
	pool->run();
	return pool;
}

template<typename T>
void threadpool<T>::run(){
	while(!m_stop){
		m_queuestat.wait();
		m_queuelocker.lock();
		if(m_workqueue.empty()){
			m_queuelocker.unlock();
			continue;
		}
		T* request = m_workqueue.front();
		m_workqueue.pop_front();
		m_queuelocker.unlock();
		if(!request)
			continue;
		request->process();
	}
}

#endif