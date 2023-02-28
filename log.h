#ifndef _LOG_
#define _LOG_
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <fstream>

// class Log
//  {
//     public: 
//     std::ofstream file;
    
//     Log() {
//         file.open("proxy.log");
//     }

    std::string getTime() {
        time_t currTime =time(0);
        char * time_c = ctime(&currTime);
        //change it to UTC time zone
        std::tm * gm =gmtime(&currTime);
        time_c = asctime(gm);
        //make it as a string
        std::string time_s(time_c);
        return time_s;
    }

    //ID: "REQUEST" from IPFROM @ TIME
    void logReq(int id , std::string request_line, std::string ip_client, std::ofstream & file) {
        //test!!!
        std::cout<<"in log req\n";
        std::string time = getTime();
        file << id << ": \"" << request_line << "\" from "<< ip_client << " @ " << time;
    }

    //ID: Responding "RESPONSE"
    void logRes(int id , std::string response_line, std::ofstream & file) {
        file << id << ": Responding \""<< response_line <<"\"\n";
    }

    //ID: not in cache
    //ID: in cache, but expired at EXPIREDTIME 
    //ID: in cache, requires validation
    //ID: in cache, valid
    void logGet(int id , int mode, std::ofstream & file) {
        if(mode == 0) {
            file << id <<": not in cache\n";
        }
        else if(mode == 1) {
            std::string time = getTime();
            file << id <<": in cache, but expired at "<< time <<"\n";
        }
        else if(mode == 2) {
            file<< id <<": in cache, requires validation\n";
        }
        else if(mode == 3) {
            file<< id <<": in cache, valid\n";
        }
    }

    //ID: Requesting "REQUEST" from SERVER
    //ID: Received "RESPONSE" from SERVER
    void logConServer(int id, std::string re_line, std::string URI, int mode, std::ofstream & file) {
        if(mode == 0) {
            file << id << ": Requesting \"" << re_line << "\" from "<< URI <<"\n";
        }
        else if(mode == 1) {
            file << id << ": Received \"" << re_line << " \" from " << URI <<"\n";
        }
    }

    //ID: ERROR MESSAGE
    void logError(std::string error, std::ofstream & file, int id = -1) {
        if(id = -1) {
            file << "(no-id): ERROR: "<< error <<"\n";
        }
        else{
            file << id <<": ERROR: "<< error <<"\n";
        }
    }

    //ID: WARNING MESSAGE
    void logWarning(std::string warning, std::ofstream & file, int id = -1) {
        if(id = -1) {
            file << "(no-id): WARNING: "<< warning <<"\n";
        }
        else{
            file << id <<": WARNING: "<< warning <<"\n";
        }
    }

    //ID: NOTE MESSAGE
    void logNote(std::string note, std::ofstream & file, int id = -1) {
        if(id = -1) {
            file << "(no-id): NOTE: "<< note <<"\n";
        }
        else{
            file << id <<": NOTE: "<< note <<"\n";
        }
    }

    //ID: Tunnel closed
    void logTunnel(int id, std::ofstream & file) {
        std::cout<<"in log tunnel\n";
        file << id << ": Tunnel closed\n";
        std::cout<<"out logtunnel\n";
    }
//     ~Log() {
//         file.close();
//     }
// };

#endif