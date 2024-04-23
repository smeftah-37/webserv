#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include<vector>
#include<cstring>
#include<cstdlib>
#include<utility>
#include <map>
#include "../Configfile/location.hpp"
#include "../Configfile/config_file.hpp"
#include "../Configfile/server.hpp"
#include "../CGI/CGI.hpp"
#include "../Request/Request.hpp"
#include <fcntl.h>
#include <sys/epoll.h>

class Response
{
    public:
        Request request;
        CGI cgi;
        std::string   setResponseLine(int fd);
std::string set_content_directory(std::string path, int fd);
        std::string   setContentType(std::string fileName, int fd);
        std::string set_response_data(int fd, int i);
        std::string getServerName();
        std::string    setDate();
int    get_response(int fd,std::map<int, std::string> &fds_socket, int *check_c,std::map<int ,Request>  &req, std::vector<Session> &sessions,std::vector<Database> datab);

        std::map<int ,Request> request_map;
        Response(const Response& other);
        Response& operator=(const Response& other);
        Response(std::map<int ,Request> rqt,int fd, int *check_c);
        Response();
        ~Response();


};


#endif