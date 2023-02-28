
#include "cache.h"

#define RESPONSE_MSG_MAX    65536

using namespace std;

mutex cache_mutex;

void Cache::moveToHead(Node * node){
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = head;
    node->next = head->next;
    head->next->prev = node;
    head->next = node;
}

void Cache::addNode(Node * node){
    node->prev = head;
    node->next = head->next;
    head->next->prev = node;
    head->next = node;
    size++;
    if(size > total_size){
        //delete tail node(tail->prev)
        Node * node = tail->prev;
        tail->prev->prev->next = tail;
        tail->prev = tail->prev->prev;
        size--;
        map_cache.erase(node->uri);
        delete node;
    }
}

string Cache::search(string uri){
    lock_guard<mutex> lg(cache_mutex);
    // If in cache, return its value(response)
    if(map_cache.count(uri)){
        Node * node = map_cache[uri];
        // Move this node to the head of the list
        moveToHead(node);
        // Return stored response
        return node->value;
    }
    // If not in cache, return an empty string
    else{
        return "";
    }
}

void Cache::add(string uri, PackResponse response){
    lock_guard<mutex> lg(cache_mutex);
    // uri exists, change its value and move to head
    if(map_cache.count(uri)){
        Node * node = map_cache[uri];
        node->value = response.response;
        moveToHead(node);
    }
    // not exists, add a new node
    else{
        Node * node = new Node(uri, response.response);
        map_cache[uri] = node;
        addNode(node);
    }
}

void Cache::store(PackResponse response, int thread_id, string uri, std::ofstream & file){
    string cache_control = response.get_cachecontrol();
    bool isPrivate = false;
    bool isNoStore = false;
    //bool isNoCache = false;
    if(cache_control != ""){
        if(cache_control.find("private") != string::npos){
            isPrivate = true;
        }
        if(cache_control.find("no-store") != string::npos){
            isNoStore = true;
        }
        // if(cache_control.find("no-cache") != string::npos){
        //     isNoCache = true;
        // }
    }
    if(!isPrivate && !isNoStore && response.code == "200"){
        // store in map_cache
        add(uri, response);
        string expires = response.get_expires(); 
        if(expires == ""){
            //LOG: cached, but requires re-validation
        }
        else{
            //LOG: cached, expires at EXPIRES
        }
    }
    if(isPrivate){
        //LOG: not cacheable because is private
    }
    if(isNoStore){
        //LOG: not cacheable because is no-store
    }
    if(response.code != "200"){
        //LOG: not cacheable because not 200 OK
    }
    return;
}

string Cache::getResfromServer(PackRequest request, int fd_server){
 send(fd_server, request.request.data(), request.request.size(), MSG_NOSIGNAL);
    vector<char> msg;
    msg.resize(RESPONSE_MSG_MAX);
    int msg_len = recv(fd_server, &msg.data()[0], msg.size(), 0);
    int index = msg_len;
    PackResponse res(msg);
    if(msg_len <= 0){
        perror("Response msg recv error");
        return res.response;
    }
    if(res.code == "304"){
        return res.response;
    }
    // If chunked
    if(res.is_chunked()){
        cout<<"in is chunked: \n";
        string tmp;
        tmp.assign(msg.begin(), msg.end());
        while(tmp.find("0\r\n\r\n") == string::npos){
            msg.resize(index + RESPONSE_MSG_MAX);
            msg_len = recv(fd_server, &msg.data()[index], RESPONSE_MSG_MAX, 0);
            if(msg_len <= 0){
                break;
            }
            tmp.assign(msg.begin() + index, msg.end() + index);
            index = index + msg_len;
        }
    }
    // If not chunked
    else{
        cout<<"in is not chunked: \n";
        int len;
        len = res.get_length();
        // Receive until index = len
        while(len > index){
            msg.resize(index + RESPONSE_MSG_MAX);
            msg_len = recv(fd_server, &msg.data()[index], RESPONSE_MSG_MAX, 0);
            if(msg_len <= 0){
                break;
            }
            index = index + msg_len;
        }
    }
    string response = string(msg.begin(), msg.end());
    return response;
}

void Cache::revalidate(PackRequest request, int fd_client, int fd_server, string uri, PackResponse response, int thread_id, std::ofstream & file){
    string last_modified = response.get_lastmodified();
    string etag = response.get_etag();
    // Send request to server and receive
    if(etag == "" && last_modified == ""){
        PackResponse res(getResfromServer(request, fd_server));
        store(res, thread_id, uri, file);
        //Send response to remote client
        cout<<"response size"<<res.response.size()<<"\n";
        int flag_s;
        // LOG: Responding "RESPONSE"
        flag_s = send(fd_client, res.response.data(), res.response.size(), MSG_NOSIGNAL);
        cout<<"response_whole : \n"<<res.response.data()<<"\n";
        cout<<"send request to remote client with flag: "<<flag_s<<"\n";
    }
    if(etag != ""){
        string req = request.request_header;
        req += "\r\nIf-None-Match: " + etag + "\r\n\r]\n";
        cout << "Etag request: " << req << endl;
        PackResponse res(getResfromServer(req, fd_server));
        // send(fd_server, req.data(), req.size(), MSG_NOSIGNAL);
        if(res.code != "304"){
            store(res, thread_id, uri, file);
        }
        cout<<"response size"<<res.response.size()<<"\n";
        int flag_s;
        // LOG: Responding "RESPONSE"
        flag_s = send(fd_client, res.response.data(), res.response.size(), MSG_NOSIGNAL);
        cout<<"response_whole : \n"<<res.response.data()<<"\n";
        cout<<"send request to remote client with flag: "<<flag_s<<"\n";
    }
    if(last_modified != ""){
        string req = request.request_header;
        req += "\r\nIf-Modified-Since: " + last_modified + "\r\n\r]\n";
        cout << "Last-Modified request: " << req << endl;
        PackResponse res(getResfromServer(req, fd_server));
        // send(fd_server, req.data(), req.size(), MSG_NOSIGNAL);
        if(res.code != "304"){
            store(res, thread_id, uri, file);
        }
        cout<<"response size"<<res.response.size()<<"\n";
        int flag_s;
        // LOG: Responding "RESPONSE"
        flag_s = send(fd_client, res.response.data(), res.response.size(), MSG_NOSIGNAL);
        cout<<"response_whole : \n"<<res.response.data()<<"\n";
        cout<<"send request to remote client with flag: "<<flag_s<<"\n";
    }
    return;
}
