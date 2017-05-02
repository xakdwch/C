#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include "zkbase.h"

class ZKSubscriber:public ZKBase {
private:
    static string m_subscribe_path;
    static void Watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);

public:
    ZKSubscriber(string host, int timeout);
    ~ZKSubscriber();

    int Subscribe();
};

string ZKSubscriber::m_subscribe_path = "/subscribe/content";

#endif
