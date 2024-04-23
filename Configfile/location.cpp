#include "location.hpp"
class location;
location::location()
{

}

location::~location()
{

}
void location::setMethods(std::vector<std::string> method)
{
    long unsigned int i;

    i = 1;
    while(i < method.size())
    {
        if(method[i] == "POST" || method[i] == "GET" || method[i] == "DELETE")
        {
            methods.push_back(method[i]);
            if(std::count(methods.begin(), methods.end(), method[i]) > 1)
            {
                std::cerr << "duplicate method" << std::endl;
                exit(1);
            }
        }
        else

            return (std::cerr << "invalid method" << std::endl, exit(1));
        i++;
    }
}
std::string location::setRet(std::vector<std::string> inde)
{
    if(inde.size() >= 2)
    {
        return (inde[1]);
    }
    std::cerr << "The return path is invalid" << std::endl;
    exit(1);

}
void location::set_gciPath(std::vector<std::string> p)
{

    if(p.size() < 2)
    {
        std::cerr <<"Invalid path of cgi script"<< std::endl; exit (1);
    }
    long unsigned int i = 1;
    while(i < p.size())
    {
        cgi_path.push_back(p[i]);
        i++;
    }

}
void    location::setExt(std::vector<std::string> e)
{
    long unsigned int i;

    i = 1;
    while(i <= e.size() - 1)
    {
        ext_cgi.push_back(e[i]);
        i++;
    }
}
std::string location::setCgistatus(std::vector<std::string> spl)
{
    if(spl.size() != 2 || (spl[1] != "on" && spl[1] != "off"))
    {
    std::cerr << "You shoud set on/off the attribute" << std::endl;
    exit(1);

    }
  return spl[1];

}
