#ifndef ZKBENCHER_H
#define ZKBENCHER_H

#include "zkbase.h"

class ZKBencher:public ZKBase {
private:
    static string m_bencher_znode;

public:
    ZKBencher(string host, int timeout);
    ~ZKBencher();
};

#endif
