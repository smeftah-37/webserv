#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "server.hpp"
#include <iostream>
#include <sys/stat.h>
#include <string>
#include <algorithm>


class location: public server
{
    public:
        bool tilda;
        std::string location_match;
        std::vector<std::string> methods;
        std::string root;
        std::string alias;
        std::string ret;
        std::vector<std::string> ext_cgi;
        std::vector<std::string> index;
        std::vector<std::string> cgi_path;
        std::string upload;
        std::string auto_index;
        std::string setRet(std::vector<std::string> inde);
        std::string status_cgi;
        void setMethods(std::vector<std::string> method);
        void    setAlias(std::vector<std::string> al);
        void set_gciPath(std::vector<std::string> p);
        void setLocamat(std::vector<std::string> l_m);
        void setExt(std::vector<std::string> e);
        std::string setCgistatus(std::vector<std::string> spl);
        location();
        ~location();

};

#endif