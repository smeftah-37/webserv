#ifndef SERVERS_HPP

#define SERVERS_HPP
#include <csignal>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <arpa/inet.h>
#include "../Configfile/config_file.hpp"
#include "ServerTCP.hpp"
#include "../Configfile/location.hpp"
#include"ServerTCP.hpp"
#include "../Request/Request.hpp"
#include <iterator>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "../response/response.hpp"
#include "../CGI/CGI.hpp"
const int TIMEOUT_SECONDS = 15;


#define RESET_COLOR "\033[0m"
#define RED_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"
#define YELLOW_COLOR "\033[33m"
#define MAGENTA_COLOR "\033[35m"
#define BLUE_COLOR "\033[34m"
#define RED_ON_WHITE "\033[31;47m"

typedef struct ep
{
    int  ep_fd;
    struct epoll_event ev;
    struct epoll_event events[1];
} Ep ;


class Servers
{
    protected:

    std::list<std::pair<std::string,std::vector<int> > >servers;
    std::map<int, time_t> lastActivityTime;
    std::map<int,std::string> fds_socket;
    std::list<std::pair<server, std::vector<location> > > search;
    std::vector<ServerTCP> serving;
    std::vector<location> loc;

    int nb_serv;
    std::map<int ,Request> *request_map;
    int epoll_fd;
    Request request;
    std::vector<int> fds_client;
    std::vector<std::pair<std::string, int> >protected_port;
    std::map<int, int > policy;
    int head;
    
    Ep ep;


    public:

    std::vector<Session> sessions;
    int register_;
    int login_;

    CGI cgi;
     std::vector<Database> database;
     int nothing;
     void handleTimeouts(int fd, int j) ;
     void display_time(time_t currentTime);
     int check_last(int first, int second);
     int checkinho(int fd);
    int main_database(std::map<std::string,std::string> &data,int fd);
    int ft_register(std::map<std::string,std::string> &data, int fd);
    int check_register(std::string first, std::string last, int fd);
    int ft_login(std::map<std::string,std::string> &data,int fd);
    Servers(config_file configfile);
    char buffer[BUFFER_SIZE+ 1];
    void handle_new_connection(int fd,std::string buffer,std::vector<std::pair<server,std::vector<location> > > &s);
    int handle_client_data(int fd,std::vector<std::pair<server,std::vector<location> > > &s);
    void handle_http_request(int fd, std::vector<std::pair<server,std::vector<location> > > &s);
    std::map<int,std::string> get_fd_socket();


    void findserver(std::vector<std::pair<server,std::vector<location> > > &s,std::string host);

    int set_socket(ServerTCP &serv);
    void add_to_epoll(int epoll_fd, int socket_fd);
    std::vector<ServerTCP> get_all_servers();
    int main_project();
    ~Servers();
    
};

#endif