#include <iostream>
#include <stdarg.h>
#include "zkbase.h"

static void zkclient_init_complete_cb(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx)
{
    if (ZOO_SESSION_EVENT == type) {
        if (ZOO_CONNECTED_STATE != state) {
            cout<<"zookeeper client connect failed!"<<endl;
            zookeeper_close(zh);
            exit(-1);
        }
    }
}

ZKBase::ZKBase(string host, int timeout)
:m_host(host),
m_timeout(timeout)
{
    m_zh = NULL;
}

ZKBase::~ZKBase()
{
    zookeeper_close(m_zh);
    //cout<<"deconstruct class ZKBase!"<<endl;
}

int ZKBase::ZKConnect()
{
    if (m_host.empty()) {
        cout<<"unknown host!"<<endl;
        return -1;
    }

    m_zh = zookeeper_init(m_host.c_str(), zkclient_init_complete_cb, m_timeout, NULL, NULL, 0);
    if (!m_zh) {
        cout<<"zookeeper_init error!"<<endl;
        return -1;
    }

    return 0;
}

int ZKBase::ZKDeleteZnode(string znode)
{
    if (znode.empty()) {
        cout<<"error znode!"<<endl;
        return -1;
    }

    int ret = zoo_delete(m_zh, znode.c_str(), -1);
    if (ret) {
        cout<<"zookeeper delete "<<znode<<" failed!"<<endl;
        return ret;
    }

    return 0;
}

int ZKBase::ZKDeleteZnode(string znode1, string znode2)
{
    if (znode1.empty() || znode2.empty()) {
        cout<<"error znode!"<<endl;
        return -1;
    }

    int ret = zoo_delete(m_zh, znode1.c_str(), -1);
    if (ret) {
        cout<<"zookeeper delete "<<znode1<<" failed!"<<endl;
        return ret;
    }

    ret = zoo_delete(m_zh, znode2.c_str(), -1);
    if (ret) {
        cout<<"zookeeper delete "<<znode2<<" failed!"<<endl;
        return ret;
    }

    return 0;
}
















