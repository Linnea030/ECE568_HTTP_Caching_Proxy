#include <stdio.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

class PackRequest {
    public:
    std::string request_info;
    std::string method;
    std::string hostname;
    std::string port;
    std::string uri;
    std::string info;
};