#ifndef _LOCKER_H_
/**
 * 同步机制封装类
*/

#include <exception>
#include <semaphore.h>
#include <pthread.h>

/***
 * 信号量
*/
class Sem {
public:
    Sem() {
        if (sem_init(&m_sem, 0, 0) != 0) {
            throw std::exception();
        }
    }

    Sem(int n) {
        if (sem_init(&m_sem, 0, n) != 0) {
            throw std::exception();
        }
    }

    ~Sem() {
        sem_destroy(&m_sem);
    }

    bool wait() { return sem_wait(&m_sem) == 0; }

    bool post() { return sem_post(&m_sem) == 0; }

private:
    sem_t m_sem;
};

/**
 * 互斥锁
*/
class Locker {
private:
    pthread_mutex_t m_mutex;
public:
    Locker() {
        if (pthread_mutex_init(&m_mutex, nullptr) != 0) {
            throw std::exception();
        }
    }

    ~Locker() {
        pthread_mutex_destroy(&m_mutex);
    }

    bool lock() {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    bool unlock() {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

    constexpr pthread_mutex_t *get() {
        return &m_mutex;
    }
};

/**
 * 条件变量
*/
class Cond {
private:
    pthread_cond_t m_cond;
public:
    Cond() {
        if (pthread_cond_init(&m_cond, nullptr) != 0) {
            throw std::exception();
        }
    }

    ~Cond() {
        pthread_cond_destroy(&m_cond);
    }

    bool wait(pthread_mutex_t *m_mutex) {
        int ret = 0;
        ret = pthread_cond_wait(&m_cond, m_mutex);
        return ret == 0;
    }

    bool time_wait(pthread_mutex_t *m_mutex, struct timespec t) {
        int ret = 0;
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
        return ret == 0;
    }

    bool signal() {
        return pthread_cond_signal(&m_cond) == 0;
    }

    bool broadcast() {
        return pthread_cond_broadcast(&m_cond) == 0;
    }
};
#endif // !_LOCKER_H_