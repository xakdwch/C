#ifndef ZKTHREAD_H
#define ZKTHREAD_H

using namespace std;

class ZKThread {
private:
    static void *entry_func(void *args);

protected:
    virtual void *entry() = 0;

public:
    ZKThread();
    virtual ~ZKThread();

    pthread_t m_tid;
    int create();
    int join(void **prval = 0);
};

#endif
