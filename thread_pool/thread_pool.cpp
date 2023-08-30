#include "thread_pool.h"

template <typename T>
ThreadPool<T>::ThreadPool(size_t thread_num, size_t max_req)
    : m_thread_num(thread_num), m_max_req(max_req) {
  m_threads = new pthread_t[m_thread_num];

  // init thread
  for (int i = 0; i < thread_num; i++) {
    std::cout << "creating the " << i + 1 << " thread\n";

    pthread_create(m_threads + i, nullptr, worker, this);

    pthread_detach(m_threads[i]);
  }
}

template <typename T>
ThreadPool<T>::~ThreadPool() {
    delete [] m_threads;
    shut_thread = true;
}

template <typename T>
bool ThreadPool<T>::append(T* work) {

    m_queue_locker.lock();
    if (m_work_queue.size() > m_max_req) {
        m_queue_locker.unlock();
        return false;
    }

    m_work_queue.push_back(work);

    m_queue_locker.unlock();

    m_queue.post();

    return true;
}

template <typename T>
void* ThreadPool<T>::worker(void* arg) {
    ThreadPool* pool = (ThreadPool*) arg;
    pool->run();
    return pool;
}

template <typename T>
void ThreadPool<T>::run() {
    while (!shut_thread) {
        m_queue.wait();

        m_queue_locker.lock();
        if (m_work_queue.empty()) {
            m_queue_locker.unlock();
            continue;
        }

        T* req = m_work_queue.front();
        m_work_queue.pop();
        m_queue_locker.unlock();

        req->process();
    }
}