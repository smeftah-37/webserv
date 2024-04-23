#include "config_file.hpp"

int config_file::checkValue(std::string str)
{
    if(str == "host" || str == "server_name" || str == "error_page"|| str == "upload"
        || str == "max_body" || str == "root"|| str == "index" || str == "listen" || str=="location")
        return (1);
    return (0);
}
int config_file::checkValue1(std::string str)
{
    if(str == "cgi_path" || str =="cgi_ext" ||str == "return" || str == "upload"
        || str == "methods" || str == "root"|| str == "index" || str == "status_cgi" || str == "autoindex")
        return (1);
    return (0);
}

location   config_file::parsingLocation(std::vector<std::string> l,std::ifstream &f,std::string &line)
{
    std::vector<std::string> spl;
    location loc;

    loc.location_match = l[1];
    getline(f,line);
        while(getline(f,line) && line.find("}") == std::string::npos)
        {
            spl = split(line,' ');
            
            if(!spl.empty() && checkValue1(spl[0]))
            {
                switch(spl[0][0])
                {
                    case 'r' : if(spl[0] == "root")
                                {
                                    if (loc.root.empty())
                                        loc.root = loc.setRoot(spl,0);
                                    else
                                    {
                                        std::cerr << "Duplicate Root" << std::endl;
                                        exit(1);
                                    }
                                }
                                else
                                {
                                    if(loc.ret.empty())
                                        loc.ret = loc.setRet(spl);
                                     else
                                    {
                                        std::cerr << "Duplicate Return" << std::endl;
                                        exit(1);
                                    }
                                }
                                break;
                                
                    case 'm' : if(loc.methods.empty())
                                     loc.setMethods(spl);
                                else
                                {
                                    std::cerr << "Duplicate methods" << std:: endl;
                                    exit(1);
                                }
                                break;
    
                    case 'i' : if(loc.index.empty()) 
                                    loc.index =    loc.setIndex(spl);
                                else
                                {
                                    std::cerr << "Duplicate index" << std:: endl;
                                    exit(1);
                                }
                                break;
                    case 'c' : if(spl[0] == "cgi_path")
                                {
                                    if(loc.cgi_path.empty())
                                        loc.set_gciPath(spl);
                                    else
                                    {
                                        std::cerr << "Duplicate path cgi" << std:: endl;
                                        exit(1);
                                    }
                                }
                                else
                                    loc.setExt(spl);
                                break;
                    case 's'  : loc.status_cgi = loc.setCgistatus(spl);
                                break;
                    case 'a'  : if(loc.auto_index.empty())
                                    loc.auto_index = loc.setCgistatus(spl);
                                else
                                {
                                    std::cerr << "Duplicate auto index" << std:: endl;
                                    exit(1);
                                }
                                break;
                    case  'u' : if (loc.upload.empty())
                                     loc.upload = loc.setRoot(spl,0);
                                else
                                {
                                    std::cerr << "Duplicate upload path" << std:: endl;
                                    exit(1);
                                }
                                break;

                }
            }
            else if(!spl.empty())
            {
                std::cerr <<"use of undifined diractive"<< line<< std::endl; exit(1);
            }
        }

        return loc;
}

config_file::config_file(std::string name)
{
    std::string line;
    std::vector<std::string> spl;
    std::ifstream f(name.c_str());
    if(!f)
    {
        std::cerr << "config file not found" << std::endl;
        exit(1);
    }
    while(std::getline(f,line))
    {
        if(line == "server{")
        {
        server obj = server();
        std::vector<location> loc;
        
            while(getline(f,line) && line != "};")
            {
                spl = split(line,' ');
              
                if(!spl.empty() && checkValue(spl[0]))
                {
                    switch(spl[0][0])
                    {
                        case 'h' : if(obj.host.empty())
                                        obj.setHostname(spl);
                                    else
                                        {
                                            std::cerr << "Duplicated host" << std::endl;
                                            exit(1);
                                        }
                                    break;
                        case 'l' : if(spl[0] == "listen")
                                    {
                                        if(obj.listen.empty())
                                            obj.setListen(spl);
                                        else
                                            {
                                                std::cerr << "Duplicated listen" << std::endl;
                                                exit(1);
                                            }
                                    }
                                        
                                    else
                                    {
                                        loc.push_back(location(parsingLocation(spl,f,line)));
                                    }
                                    break;
                        case 's' : if(obj.server_name.empty())
                                        obj.setServername(spl);
                                    else
                                        {
                                            std::cerr << "Duplicated Server name" << std::endl;
                                            exit(1);
                                        }
                                    break;
                        case 'm' : if(obj.max_body == 0)
                                        obj.setMaxbody(spl[1]);
                                    else
                                        {
                                            std::cerr << "Duplicated Max body " << std::endl;
                                            exit(1);
                                        }
                                    break;
                        case 'r' : if(obj.root.empty())
                                        obj.setRoot(spl,1);
                                    else
                                    {
                                            std::cerr << "Duplicated root" << std::endl;
                                            exit(1);
                                    }
                                    break;
                        case 'i' : if(obj.index.empty())
                                        obj.setIndex(spl);
                                    else
                                    {
                                        std::cerr << "Duplicated index " << std::endl;
                                        exit(1);
                                    }

                                    break;
                        case  'e' : obj.setError(spl);
                                    break;
                        case  'u' : if(obj.upload.empty())
                                        obj.upload = obj.setRoot(spl,0);
                                    else
                                    {
                                        std::cerr << "Duplicated Root " << std::endl;
                                        exit(1);
                                    }
                                    break;
                        
                    }
                }
                else if(!spl.empty())
                {
                   std::cerr <<"++use of undifined diractive  "<< line<< std::endl; exit(1);
                }
            } 
            if(obj.listen.empty() || obj.host.empty() || obj.root.empty())
            {
                std::cerr << "You should set the host and the port and root" << std::endl;
                exit(1);
            }
            serv.push_back(std::make_pair(obj,loc));
                std::vector <std::string > locatelli;
            for (unsigned long int i = 0 ; i < loc.size();i++)
            {
                locatelli.push_back(loc[i].location_match);
                if(std::count(locatelli.begin(), locatelli.end(), locatelli[i]) > 1)
                {
                    std::cerr << "Duplicated location match" << std::endl;
                    exit(1);
                }
            }  
            
        }
        file +=line;
    }
    f.close();
    std::list<std::pair<server, std::vector<location> > >::iterator it;
    for (it = serv.begin(); it != serv.end(); ++it) {
        server& s = it->first;
        std::list<std::pair<server, std::vector<location> > >::iterator i;

        it++;
        for (i = it; i != serv.end(); ++i) {
            server& tmp = i->first;
            if(tmp.server_name == s.server_name)
            {
                std::cerr << "duplicated Server name" << std::endl;
                exit(1);
            }
        
    }
    it--;

        
    }
    if(serv.size() ==0)
    {
        std::cerr << "Config file not valid" << std::endl;
        exit(1);
    }
}


void config_file::parsing_server()
{
}
config_file::~config_file()
{

}