#include "package.h"

void PackRequest::parse_req() {
    this->parse_line();
    this->parse_header();
    //this->parse_body();
}

//parse line to get method and url
void PackRequest::parse_line() {
    //get request line
    size_t pos_line = request.find("\r\n");
	request_line = request.substr(0, pos_line);
    //get method in line
    size_t pos_mend = request_line.find(" ");
	method = request_line.substr(0, pos_mend);
    //test!!!
    std::cout<<"in parse_line get method: "<<method<<std::endl;
    //get hostname:port in line
	size_t pos_u = request_line.find(" ", pos_mend + 1);
	URI = request_line.substr(pos_mend + 1, pos_u - pos_mend);
}

void PackRequest::parse_header() {
    //get request header
    size_t pos_line = request.find("\r\n");
    size_t pos_header = request.find("\r\n\r\n");
    //if no \r\n\r\n
    if(pos_header == std::string::npos) {
        port = "";
        hostname = "";
        return;
    }
	request_header = request.substr(pos_line + strlen("\r\n"), pos_header);
    size_t pos_h = request.find("Host");
    size_t len_host = strlen("Host: ");
	std::string h1 = request.substr(pos_h + len_host);
	size_t end_line = h1.find("\r\n");
	std::string hp = h1.substr(0, end_line);
	size_t pos_p = hp.find(":");
	//std::cout << port_pos <<std::endl;
	if(pos_p == std::string::npos){
		hostname = hp;
		port="80";
	} else{
        hostname = hp.substr(0, pos_p);
		port = hp.substr(pos_p + 1);
	}
}

void PackRequest::parse_body() {
    size_t pos_header = request.find("\r\n\r\n");
	std::string info = request.substr(pos_header + 4);
	size_t pos_len = request_header.find("Content-Length");
    if (pos_len == std::string::npos) {
        content_len_remain = -1;
    }
	std::string content_len;
    content_len = request_header.substr(pos_len + 16);
	size_t pos_len_end = content_len.find("\r\n");
	len_info = content_len.substr(0, pos_len_end);

    int rest_len = request.size() - int(pos_header) - 8;
    size_t end = request.find("\r\n", pos_len);
    //len_info = stoi(request.substr(pos_len + 8, end - pos_len - 6));
    content_len_remain = stoi(len_info) - rest_len - 4;
}

void PackRequest::parse_cache() {
  size_t pos_noc = request.find("no-cache");
  if (pos_noc == std::string::npos) {
    flag_nocache = 0;
  }
  else {
    flag_nocache = 1;
  }
}

void PackRequest::print_request() {
    std::cout<<"Here is request info:\n";
    std::cout<<"request_line: " << request_line <<"\n";
    std::cout<<"request_header: " << request_header <<"\n";
    //std::cout<<"request_body: " << request_body <<"\n";
    //std::cout<<"request_URI: " << URI <<"\n";
    std::cout<<"request_method: " << method <<"\n";
    std::cout<<"request_hostname: " << hostname <<"\n";
    std::cout<<"request_port: " << port <<"\n";
    std::cout<<"len_info: " << len_info <<"\n";
    //std::cout<<"request_info: " << info <<"\n";
}

//========== Response ==========

void PackResponse::parse_res() {
    parse_header();
    parse_status();
}

void PackResponse::parse_header(){
    size_t pos_line = response.find("\r\n");
	response_line = response.substr(0, pos_line);
    //get uri by first line
    URI = response_line;
    size_t pos_h_end = response.find("\r\n\r\n");
    //get header
    //test!!!
    response_header = response.substr(0, pos_h_end);
    size_t len = strlen("\r\n\r\n");
    //get body
    response_body = response.substr(pos_h_end + len);
}

void PackResponse::parse_status(){
    //get position
    size_t pos_st = response_header.find(" ");
    size_t pos_code_end = response_header.find(" ", pos_st + 1);
    //get code
    size_t pos_st_end = response_header.find("\r\n");
    status_code = response_header.substr(pos_st + 1, pos_st_end - pos_st - 1);
    code = response_header.substr(pos_st + 1, pos_code_end - pos_st - 1);
}

bool PackResponse::is_chunked(){
    //size_t pos = response.find("chunked");
    // if(response.find("chunked") != std::string::npos){
    //     return true;
    // }
    return (response.find("chunked") != std::string::npos) ? true : false;
}

int PackResponse::get_length(){
    std::string name = "Content-Length";
    size_t pos = response.find(name);
    if(pos == std::string::npos) {
        std::cout<<"no content-length\n";
        return 0;
    }
    //if not found?
    size_t pos_end = response.find("\r\n", pos + 1);
    std::string len_s = response.substr(pos + 2 + name.size(), pos_end - pos - name.size() - 2);
    std::cout<<"in get_length, len_s: "<<len_s<<"\n";
    return stoi(len_s);
}

std::string PackResponse::finder(std::string str){
    size_t pos = response.find(str);
    if(pos == std::string::npos) {
        return "";
    }
    size_t pos_end = response.find("\r\n", pos + 1);
    // 2: ": "
    std::string res = response.substr(pos + 2 + str.size(), pos_end - 2 - pos - str.size());
    return res;
}

std::string PackResponse::get_cachecontrol(){
    return finder("Cache-Control");
}

std::string PackResponse::get_expires(){
    return finder("Expires");
}

std::string PackResponse::get_etag(){
    return finder("ETag");
}

std::string PackResponse::get_lastmodified(){
    return finder("Last-Modified");
}
