#include <iostream>
#include <pthread.h>
#include "zkthread.h"

ZKThread::ZKThread()
{
    m_tid = 0;
}

ZKThread::~ZKThread()
{
    //cout<<"deconstruct ZKThread!"<<endl;
}

void *ZKThread::entry_func(void *args)
{
    return ((ZKThread *)args)->entry();
}

int ZKThread::create()
{
    int ret = pthread_create(&m_tid, NULL, entry_func, (void *)this);
    if (ret) {
        cout<<"create thread failed with error: "<<ret<<endl;
    }

    return ret;
}

int ZKThread::join(void **prval)
{
    if (!m_tid) {
        cout<<"join on thread that never started!"<<endl;
        return -1;
    }

    int status = pthread_join(m_tid, prval);
    if (status) {
         cout<<"join on thread "<<m_tid<<" failed with error: "<<status<<endl;
    }
    else {
        //cout<<"thread "<<m_tid<<" exiting!"<<endl;
        m_tid = 0;
    }

    return status;
}
