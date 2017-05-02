#include <iostream>
#include "zkbencher.h"

ZKBencher::ZKBencher(string host, int timeout):ZKBase(host, timeout)
{}

ZKBencher::~ZKBencher()
{}

void usage()
{
    cout<<"Options:"<<endl;
    cout<<"  -h, --help           show this help message and exit"<<endl;
    cout<<"  --server=SERVERS     comma separated list of host:port (default localhost:2181)"<<endl;
    cout<<"  --timeout=TIMEOUT    session timeout in milliseconds (default 3000)"<<endl;
    cout<<"  --rootznode=ZNODE    znode for the test (default /zkbencher)"<<endl;
    cout<<"  --znodesize=SIZE     data size when creating/seting znodes (default 25)"<<endl;
    cout<<"  --znodecount=COUNT   the number of znodes to operate each time (default 10000)"<<endl;
    cout<<"  --synchronous        by default asynchronous zookeeper api is used, this forces synchronous calls"<<endl;
}

int parseArgv(int argc, const char *argv[], vector<const char*>&args)
{
    int i;

    for (i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }

    return 0;
}

int main(int argc, const char *argv[])
{
    vector<const char*>args;
    parseArgv(argc, argv, args);

    vector<const char*>::iterator it = args.begin();
    while (it != args.end()) {
        cout<<(*it)<<endl;
        it++;
    }

    if (args.empty()) {
        usage();
    }

    return 0;
}
