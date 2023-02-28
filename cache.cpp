
#include "cache.h"

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

void Cache::store(PackResponse response, int thread_id, string uri){
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
