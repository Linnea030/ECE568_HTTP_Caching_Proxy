#include <stdio.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

class PackRequest {
    public:
    std::string whole;
    std::string request;
    std::string request_line;
    std::string request_header;
    std::string request_body;
    //std::string info;
    std::string method;
    //std::string URI;
    std::string hostname;
    std::string port;
    std::string len_info;

    int content_len_remain;
    int flag_nocache;

    PackRequest(std::string request_info_s) {
        request = request_info_s;
        parse();
    }

    void parse();
    void parse_line();
    void parse_header();
    void parse_cache();
    void parse_body();
    void print_request();
    
    ~PackRequest() {}
};

class PackResponse {
    public:
    std::string response;
    std::string response_line;
    std::string response_header;
    std::string response_body;
    std::string method;
    std::string hostname;
    std::string port;
    std::string info;

    PackResponse(std::string response_info_s) {
        response = response_info_s;
        //parse();
    }

    //void parse();
    //void parse_header();
    
    ~PackResponse() {}
};