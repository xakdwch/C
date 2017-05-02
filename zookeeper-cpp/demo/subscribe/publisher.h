#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "zkbase.h"

class ZKPublisher:public ZKBase {
private:
    static string m_subscribe_prefix;
    static string m_subscribe_path;

public:
    ZKPublisher(string host, int timeout);
    ~ZKPublisher();

    int Publish();
    int Clean();
};

string ZKPublisher::m_subscribe_prefix = "/subscribe";
string ZKPublisher::m_subscribe_path = "/subscribe/content";

#endif
