#ifndef PRODUCER_H
#define PRODUCER_H

#include <set>
#include <vector>
#include "zkbase.h"
#include "zkthread.h"

class ZKQueue:public ZKBase {
private:
    static string m_qprefix;
    static string m_qpath;

    void DisplayItems();
    void GetPrevItem(int item_seq, string &seq_str);
    static void WatchExistsCB(zhandle_t *zh, int type, int stat, const char *path, void *watcherCtx);

    struct WorkThread:public ZKThread {
        int done;
        char *item;
        ZKQueue *producer;
        WorkThread(ZKQueue *producer):done(0),item(NULL),producer(producer){};
        void *entry() {
            producer->Worker(this);
            return 0;
        }
        void SetWorkThreadDone() {done = 1;}
        int IsWorkThreadDone() {return done;}
        void SetItem(char *path) {item = path;}
    };

    unsigned int ready;
    void SetThreadReady(){++ready;}
    int IsAllThreadReady() {return (ready == work_set.size() ? 1 : 0);}

    int vec_ready;
    vector<int> seq_vec;
    void SetVecReady() {vec_ready = 1;}
    int IsVecReady() {return vec_ready;}

    set<WorkThread*> work_set;
    void *Worker(WorkThread *wt);
    void Start();
    void Stop();

public:
    ZKQueue(string host, int timeout);
    virtual ~ZKQueue();

    int Produce();
    int Clean();
};

string ZKQueue::m_qprefix = "/distri-queue";
string ZKQueue::m_qpath = m_qprefix + "/";

#endif
