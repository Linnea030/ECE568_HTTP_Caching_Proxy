#ifndef _HANDLER_
#define _HANDLER_

//#include "proxy.h"
#include "package.h"
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <thread>
#include <string>


class Cache{};


class Handler{
    public:
    void GETHandler(PackRequest request, int fd_client, int fd_server, Cache & cache, int thread_id);
};

#endif