#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

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
#include<list>

#include "server.hpp"
#include "location.hpp"

class server;

class config_file
{
public:
    int fd;

    std::list<std::pair<server,std::vector<location> > >serv;
    std::string file;
    config_file(std::string name);
   location   parsingLocation(std::vector<std::string> l,std::ifstream &f,std::string &line);
    void    parsing_server();
    int checkValue(std::string str);
    int checkValue1(std::string str);

    ~config_file();
};



#endif