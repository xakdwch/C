#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <algorithm>
#include "zkfifo.h"

ZKQueue::ZKQueue(string host, int timeout)
:ZKBase(host, timeout)
{
    ready = 0;
    vec_ready = 0;
}

ZKQueue::~ZKQueue()
{
    //cout<<"deconstruct class ZKQueue!"<<endl;
}

void ZKQueue::DisplayItems()
{
    struct String_vector children;

    int ret = zoo_get_children(m_zh, m_qprefix.c_str(), 0, &children);
    if (ret) {
        cout<<"zoo_get_children "<<m_qprefix.c_str()<<" failed!"<<endl;
    }

    int i;
    cout<<"----------znode stat----------"<<endl;
    for(i = 0; i < children.count; i++) {
        cout<<"znode["<<i<<"]: "<<children.data[i]<<endl;
    }
}

void ZKQueue::GetPrevItem(int item_seq, string &seq_str)
{
    set<WorkThread*>::iterator it = work_set.begin();
    while (it != work_set.end()) {
        if (atoi((*it)->item) == item_seq) {
            seq_str = m_qpath + (*it)->item;
            return;
        }
        ++it;
    }

    assert(0);
}

void ZKQueue::WatchExistsCB(zhandle_t *zh, int type, int stat, const char *path, void *watcherCtx)
{
    WorkThread *wt = static_cast<WorkThread*>(watcherCtx);
    string self_path = m_qpath + wt->item;

    if (ZOO_DELETED_EVENT == type) {
        char data[10] = {0};
        int len = sizeof(data);
        assert(!zoo_get(zh, self_path.c_str(), 0, data, &len, NULL));
        cout<<"Thread "<<wt->m_tid<<" consume "<<self_path<<", znode data: "<<data<<endl;
        assert(!zoo_delete(zh, self_path.c_str(), -1));
        wt->SetWorkThreadDone();

        return;
    }

    cout<<"Unexpected event: "<<type<<endl;
    assert(0);
}

void *ZKQueue::Worker(WorkThread *wt)
{
    struct Stat stat;

    while (!IsVecReady()) {
        usleep(100);
    }

    int self_seq = atoi(wt->item);
    if (self_seq != seq_vec[0]) {
        unsigned int i;
        int prev_seq = -1;
        for (i = 1; i < seq_vec.size(); i++) {
            if (self_seq == seq_vec[i]) {
                prev_seq = seq_vec[i - 1];
                string prev_str;
                GetPrevItem(prev_seq, prev_str);

                assert(!zoo_wexists(m_zh, prev_str.c_str(), WatchExistsCB, (void *)wt, &stat));
                SetThreadReady();
                break;
            }
        }
        assert(i != seq_vec.size());
    }
    else {
        SetThreadReady();
        while (!IsAllThreadReady()) {
            usleep(100);
        }

        char data[10] = {0};
        int len = sizeof(data);
        string self_str = m_qpath + wt->item;
        assert(!zoo_get(m_zh, self_str.c_str(), 0, data, &len, NULL));
        cout<<"Thread "<<wt->m_tid<<" consume "<<self_str<<", znode data: "<<data<<endl;
        assert(!zoo_delete(m_zh, self_str.c_str(), -1));
        wt->SetWorkThreadDone();
    }

    while (!wt->IsWorkThreadDone()) {
        usleep(100);
    }

    return NULL;
}

void ZKQueue::Start()
{
    struct String_vector children;
    struct Stat stat;

    int ret = zoo_get_children2(m_zh, m_qprefix.c_str(), 0, &children, &stat);
    if (ret) {
        cout<<"zoo_get_children2 failed!"<<endl;
        return;
    }

    int i;
    for (i = 0; i < children.count; i++) {
        seq_vec.push_back(atoi(children.data[i]));
        WorkThread *wt = new WorkThread(this);
        wt->create();
        wt->SetItem(children.data[i]);
        work_set.insert(wt);
    }

    sort(seq_vec.begin(), seq_vec.end());
    SetVecReady();
}

void ZKQueue::Stop()
{
    set<WorkThread*>::iterator it = work_set.begin();
    while (it != work_set.end()) {
        (*it)->join();
        delete(*it);
        work_set.erase(it);
        it++;
    }
}

int ZKQueue::Produce()
{
    if (m_qpath.empty()) {
        cout<<"unknown path!"<<endl;
        return -1;
    }

    int ret = zoo_create(m_zh, m_qprefix.c_str(), NULL, 0, &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0);
    if (ret && ZNODEEXISTS != ret) {
        cout<<"zoo_create "<<m_qprefix<<" failed!"<<endl;
        return -1;
    }

    int i;
    char data[10] = {0};
    char path[125] = {0};
    for (i = 0; i < 10; i++) {
        snprintf(data, sizeof(data) - 1, "%d", i);
        assert(!zoo_create(m_zh, m_qpath.c_str(), data, strlen(data), &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL|ZOO_SEQUENCE, path, sizeof(path)));
        cout<<"Create znode "<<path<<endl;
    }

    DisplayItems();
    Start();
    Stop();

    return 0;
}

int ZKQueue::Clean()
{
    return ZKDeleteZnode(m_qprefix);
}

int main(int argc, const char *argv[])
{
    ZKQueue *producer = new ZKQueue("20.2.37.210:2181", 3000);

    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
    if (producer->ZKConnect()) {
         delete(producer);
         return -1;
    }

    if (producer->Produce()) {
        delete(producer);
        return -1;
    }

    cout<<"Click enter and exit!"<<endl;
    getchar();
    producer->Clean();

    delete(producer);
    return 0;
}
