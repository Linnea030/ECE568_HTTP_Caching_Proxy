#include "handler.h"
#include "package.h"
#include "socket_info.h"
#include "csbuild.h"

#define RESPONSE_MSG_MAX    65536

using namespace std;

/**
 * @brief Handle GET method
 * 
 * @param request: client request info
 * @param fd_client: remote client fd
 * @param fd_server: remote server fd
 * 
 * @param cache: cache相关info
 * 
 * @param thread_id: 线程id
 * 
 * @return null
 */
void Handler::GETHandler(PackRequest req, int fd_client, int fd_server, Cache & cache, int thread_id){
    std::cout<<"int get handler\n";
    //string hostname = req.hostname;
    //string portnum = req.port;
    string uri = req.URI; //判断cache
    // 作为client与remote server连接: fd_server
    // int client_fd;
    // client_fd = init_client(hostname, portnum);
    // 先在cache里面找
    // 待补充：在cache里面
    bool in_cache = false;
    if(in_cache){}
    // If not in cache
    else{
        std::cout<<"not in cache\n";
        //待补充：写入log: not in cache
        //读取client的request，send给remote server
        //待补充：写入log：request from server
        //client接受返回消息：判断chunk！
        //处理response
        //send回client

        //send request to remote server
        int flag_s;
        std::cout<<"req.request.data():\n"<<req.request.data()<<"\n";
        flag_s = send(fd_server, req.request.data(), req.request.size(), MSG_NOSIGNAL);
        std::cout<<"send request to remote server with flag: "<<flag_s<<"\n";

        //receive response
        vector<char> msg;
        msg.resize(RESPONSE_MSG_MAX);
        // receive the first response package
        int msg_len = recv(fd_server, &msg.data()[0], msg.size(), 0);
        if(msg_len <= 0){
            perror("Response msg recv error");
            return;
        }
        std::cout<<"recv response from remote server with flag: "<<msg_len<<"\n";

        int index = msg_len;
        // 待补充：把第一个包存入response中
        //string response_info_s(msg);
        PackResponse res(msg);

        //处理code 304？
        if(res.code == "304"){
            std::cout<<"get 304 !!!\n";
            return;
        }

        // 待补充：if res.is_chunked()
        if(res.is_chunked()){
            std::cout<<"in is chunked: \n";
            string tmp;
            //tmp.insert(tmp.begin(), msg.begin(), msg.end());
            tmp.assign(msg.begin(), msg.end());
            // 这个边界条件什么意思？ Chunk结束条件？
            while(tmp.find("0\r\n\r\n") == string::npos){
                msg.resize(index + RESPONSE_MSG_MAX);
                msg_len = recv(fd_server, &msg.data()[index], RESPONSE_MSG_MAX, 0);
                if(msg_len <= 0){
                    break;
                }
                tmp.assign(msg.begin() + index, msg.end() + index);
                index = index + msg_len;
            }
            return;
        }
        // If not chunked
        else{
            std::cout<<"in is not chunked: \n";
            int len;
            // 待补充：len = res.getLen();
            len = res.get_length();
            // Receive until index = len
            std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
            while(len > index){
                msg.resize(index + RESPONSE_MSG_MAX);
                // recv size应为65536还是msg.size()?
                msg_len = recv(fd_server, &msg.data()[index], RESPONSE_MSG_MAX, 0);
                std::cout<<msg.data();
                if(msg_len <= 0){
                    break;
                }
                index = index + msg_len;
            }
            std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        }

        // 待补充：把结果存入response中
        // string response_info_s(msg);
        PackResponse response(msg);

        // 待补充：receive log
        // 待补充：response存入cache

        //Send response to remote client
        string response_whole;
        response_whole = string(msg.begin(), msg.end());
        std::cout<<"response size"<<response_whole.size()<<"\n";
        flag_s = send(fd_client, response_whole.data(), response_whole.size(), MSG_NOSIGNAL);
        std::cout<<"response_whole : \n"<<response_whole.data()<<"\n";
        std::cout<<"send request to remote client with flag: "<<flag_s<<"\n";
    }
}
