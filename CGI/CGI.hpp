#ifndef CGI_HPP
#define CGI_HPP

#include<iostream>
#include<cstdlib>
#include<unistd.h>
#include"../Request/Request.hpp"



class CGI{
    std::string port;
    std::string host;
    int fd;
    Request request;
    std::string ext;
    std::map<std::string, std::string > env_map;
    std::string full_path;
    public:
        CGI();
        CGI(Request *requestinho);
        CGI(const CGI& other);
        CGI& operator=(const CGI& other);
        int start_CGI(std::vector<Database> data, int value, int &t_out,std::map<std::string,int> &removing_m, int &child_process,Request &r);
        int launch_cgi(int &t_out,std::map<std::string,int> &removing_m, int &child_process,Request &r);
        ~CGI();

};

#endif