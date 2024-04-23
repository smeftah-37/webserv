#ifndef SERVER_HPP
#define SERVER_HPP


#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <dirent.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

class server
{
  
    public:
        std::vector<int> listen;
        std::vector<std::string> server_name;
        std::map<int, std::string> error;
        unsigned long max_body;
        std::string host;
        std::string root;
        std::vector<std::string> index;
        std::string upload;
        int CheckError(std::string& value, int s);
        void setListen(std::vector<std::string> l);
        void setServername(std::vector<std::string> name);
        void setHostname(std::vector<std::string> h);
        void   setMaxbody(std::string size);
        std::string    setRoot(std::vector<std::string> r, int ch);
        std::vector<std::string> setIndex(std::vector<std::string> inde);
        void setError(std::vector<std::string>  err);    
        bool operator<(const server& other) const;
        server();
        ~server();
};
std::vector<std::string> split(std::string str, char delimiter);
#endif