#ifndef ZKCLIENT_H
#define ZKCLIENT_H

#include <vector>
#include "zookeeper.h"
#include "zookeeper.jute.h"

using namespace std;

class ZKBase {
private:
    string m_host;
    int m_timeout;

public:
    ZKBase(string host, int timeout);
    virtual ~ZKBase();

    zhandle_t * m_zh;
    int ZKConnect();
    int ZKDeleteZnode(string znode);
    int ZKDeleteZnode(string znode1, string znode2);
};

#endif
