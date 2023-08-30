#ifndef _THREADPOOL_H

#include <queue>
#include <pthread.h>

#include "../locker/locker.h"

template <typename T>
class ThreadPool {
private:
    size_t m_thread_num;

    pthread_t* m_threads;

    size_t m_max_req;

    std::queue<T*> m_work_queue;

    Locker m_queue_locker;

    Sem m_queue;

    bool shut_thread;

    static void* worker(void* arg);

    void run();

public:

    ThreadPool() {}
    ~ThreadPool() {}

    ThreadPool(size_t thread_num = 8, size_t max_req = 100000);
    bool append(T* work);
};

#endif // !_THREADPOOL_H