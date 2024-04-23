
#include "server.hpp"
#include <algorithm>
#include <limits.h>
class server;
server::server()
{
    max_body = 0;
}
server::~server()
{

}
    bool server::operator<(const server& other) const {
        return max_body < other.max_body;
    }
std::vector<std::string> split(std::string str, char delimiter)
{
    std::vector<std::string> ret;
    std::istringstream conv(str);
    std::string buffer;
    while(getline(conv,buffer,delimiter))
    {
        if(!buffer.empty())
            ret.push_back(buffer);   
    }
    return ret;
}
int server::CheckError(std::string& value, int s)
{
    int k;

        k = 0;
        if (s == 1)
            while(value[k] && std::isalpha(value[k]) && k++>= 0);
        else if (s == 0)
            while(value[k]  &&  std::isdigit(value[k]) && k++>= 0);
        else
        {
            if(value[k])
                k = value.length();
        }
        if (k == static_cast<int>(value.length()) && k != 0)
            return (1);
        else
            return (0);
}
void    server::setListen(std::vector<std::string> l)
{
    long unsigned int i;
    i = 0;
    while( i < l.size() - 1)
    {
        if(CheckError(l[i + 1],0) && atoi(l[i+1].c_str())> 0 && atoi(l[i + 1].c_str()) <= 65535)
        {
            for(unsigned long j = 0; j < listen.size() ; j++)
            {
                if(listen[j] == atoi(l[i + 1].c_str()))
                {
                   std::cerr <<"Duplicated port" << std::endl;
                    exit(1);
                }
            }
            listen.push_back(atoi(l[i + 1].c_str()));
        }
        else
        {
            std::cerr << "Invalid port | out of range"<< std::endl;
            exit(1);
        }
        i++;

    }
}
std::string server::setRoot(std::vector<std::string> r, int chk)
{
    DIR *dir;


    dir=opendir(r[1].c_str());
    if(r.size() != 2 || !dir)
    {
        std::cerr << "404" << std::endl;
        exit(1);
    }
    if(chk == 1)
        root = r[1];
    closedir(dir);
    return r[1];

}

std::vector<std::string> server::setIndex(std::vector<std::string> inde)
{
    long unsigned int i;

    i = 1;
    while(i < inde.size())
    {
        index.push_back(inde[i++]);
        
    }
    return index;
}
void server::setServername(std::vector<std::string> name)
{
    long unsigned int i;

    i = 1;
    while(i <= name.size() - 1)
    {
        server_name.push_back(name[i]);
        i++;
    }
}
void   server::setMaxbody(std::string size)
{
    if(CheckError(size,0))
    {
        char *ptr;
        long result = strtol(size.c_str(),&ptr,10);
        if ((result > 10000000) )
        {
            std::cerr << "max body > 10,000,000" << std::endl;
            exit(1);
        } 
        max_body = static_cast<unsigned long>(result);
    }
        
    else
    {
        std::cerr << "maxBodysize not valid" <<std::endl ;
        exit(1);
    }
}
void server::setError(std::vector<std::string>  err)   
{
    if(err.size() != 3)
    {
        std::cerr << "error_page " << std::endl;
        exit(1);
    }
        if(CheckError(err[1], 0) && !CheckError(err[2],0))
            error[atoi(err[1].c_str())] = err[2];
  
}
void server::setHostname(std::vector<std::string> h)
{
    int i;

    i = 0;
    std::vector<std::string> tmp;
    if(h.size() != 2)
        return (std::cerr << "Host Error" << std::endl, exit(1));
    tmp = split(h[1],'.');
    while(i < 4)
    {
        if(tmp.size() == 4 && CheckError(tmp[i], 0)  && atoi(tmp[i].c_str()) >=0 && atoi(tmp[i].c_str())<=255)
            i++;
        else
            return(std::cerr << "Host adress not valid"<<std::endl, exit(1));
    }
    host = h[1];
}
