#ifndef LOCK_H_
#define LOCK_H_

#include <pthread.h>

namespace levelque {
    class CommLock
    {
    private:
        pthread_mutex_t _lock;
    public:
        CommLock()
        {
            pthread_mutex_init(&_lock,NULL);
        }
        ~CommLock(){}
        void Lock()
        {
            pthread_mutex_lock(&_lock);
        }
        void UnLock()
        {
            pthread_mutex_unlock(&_lock);
        }
    };

    class Lock{
        CommLock* _lock;
    public:
        Lock(CommLock * lock)
        {
            _lock = lock;
            _lock->Lock();
        }

        ~Lock()
        {
            _lock->UnLock();
        }
    };
}
#endif
