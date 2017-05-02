#include <iostream>
#include <assert.h>
#include "publisher.h"

static string origi_content = "{server:\"20.2.37.208\",logname:\"root\",passwd:\"123456\"}";

ZKPublisher::ZKPublisher(string host, int timeout):ZKBase(host, timeout)
{}

ZKPublisher::~ZKPublisher()
{}

int ZKPublisher::Publish()
{
    int ret = zoo_create(m_zh, m_subscribe_prefix.c_str(), NULL, 0, &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0);
    if (ret && ZNODEEXISTS != ret) {
        cout<<"zookeeper create "<<m_subscribe_prefix.c_str()<<" failed!"<<endl;
    }

    ret = zoo_create(m_zh, m_subscribe_path.c_str(), origi_content.c_str(), origi_content.size(), &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0);
    if (ret && ZNODEEXISTS != ret) {
        cout<<"zookeeper create "<<m_subscribe_path.c_str()<<" failed!"<<endl;
        return -1;
    }

    cout<<"Publish Content: ";
    string line;
    while (getline(cin, line)) {
        assert(!zoo_set(m_zh, m_subscribe_path.c_str(), line.c_str(), line.size(), -1));
        cout<<"Publish Content: ";
    }

    return 0;
}

int ZKPublisher::Clean()
{
    return ZKDeleteZnode(m_subscribe_path, m_subscribe_prefix);
}

int main(int argc, char *argv[])
{
    ZKPublisher *publisher = new ZKPublisher("20.2.37.208:2181", 3000);

    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
    if (publisher->ZKConnect()) {
        delete(publisher);
        return -1;
    }

    if (publisher->Publish()) {
        delete(publisher);
        return -1;
    }

    publisher->Clean();
    delete(publisher);
    return 0;
}
