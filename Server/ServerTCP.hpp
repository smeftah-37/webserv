#ifndef SERVERTCP_HPP
#define SERVERTCP_HPP

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
#include <arpa/inet.h>
#include "../Configfile/config_file.hpp"
#include "../Configfile/location.hpp"

class ServerTCP
{
    private:
        std::string host;
        
        int port;
        int domain;
        int service; 
        int protocol;
        int backlog; 
        int fd_socket; 
        std::vector<location> loc;
        

    public:
        std::vector<int> ports;
        struct sockaddr_in address;
        std::vector<int> fds_socket;
        std::vector<int>cls_socket;
        ServerTCP(std::string host, std::vector<int> ports, std::vector<location> loc);
        int get_protocol();
        int get_domain();
        int get_service();
        int get_backlog();
        void set_port(int i);
        std::vector<int> get_port();
        std::string get_host();
        int get_fd_socket(int i);
        struct sockaddr_in *get_address();
        int get_porting();
        ~ServerTCP();

};


#endif