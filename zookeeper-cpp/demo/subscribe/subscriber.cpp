#include <iostream>
#include <assert.h>
#include "subscriber.h"

ZKSubscriber::ZKSubscriber(string host, int timeout):ZKBase(host, timeout)
{}

ZKSubscriber::~ZKSubscriber()
{}

void ZKSubscriber::Watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx)
{
    if (ZOO_CHANGED_EVENT == type) {
        if (ZOO_CONNECTED_STATE == state) {
            char buffer[1024] = {0};
            int buflen = sizeof(buffer);
            ZKSubscriber *subscriber = static_cast<ZKSubscriber*>(watcherCtx);

            assert(!zoo_wget(subscriber->m_zh, m_subscribe_path.c_str(), Watcher, (void *)subscriber, buffer, &buflen, NULL));
            cout<<"New Content: "<<buffer<<endl;
        }
        else {
            cout<<"Unexpected state: "<<state<<endl;
        }
    }
    else {
        cout<<"Unexpected event: "<<type<<endl;
    }
}

int ZKSubscriber::Subscribe()
{
    char buffer[1024] = {0};
    int buflen = sizeof(buffer);

    if (zoo_wget(m_zh, m_subscribe_path.c_str(), Watcher, (void *)this, buffer, &buflen, NULL)) {
        cout<<"zookeeper get "<<m_subscribe_path<<" failed!"<<endl;
        return -1;
    }

    cout<<"Get Content: "<<buffer<<endl;

    getchar();

    return 0;
}

int main(int argc, char *argv[])
{
    ZKSubscriber *subscriber = new ZKSubscriber("20.2.37.208:2181", 3000);

    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
    if (subscriber->ZKConnect()) {
        delete(subscriber);
        return -1;
    }

    if (subscriber->Subscribe()) {
        delete(subscriber);
        return -1;
    }

    delete(subscriber);
    return 0;
}
