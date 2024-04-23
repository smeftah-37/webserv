#include "CGI.hpp"
#include<vector>
#include <sys/wait.h>
#include <csignal>

CGI::CGI()
{

}
CGI::CGI(Request *requestinho)
{
   this->request = *requestinho;
   ext = requestinho->ext;
   fd = -1;
   if(requestinho->path_cgi.size() > 0)
        full_path = requestinho->path_cgi[0];

    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = request.variables.begin(); it != request.variables.end(); ++it) 
    {
        if(it->first == "Host")
        {
            std::vector<std::string> parser = split(it->second,':');
            this->host = parser[0];
            this->port = parser[1];
        }
    }
    
}
CGI::CGI(const CGI& other) {
    port = other.port;
    fd = other.fd;
    host = other.host;
    request = other.request; 
    ext = other.ext;
    env_map = other.env_map;
    full_path = other.full_path;
}


CGI& CGI::operator=(const CGI& other) {
    if (this != &other) {
        port = other.port;
        host = other.host;
        request = other.request;
        ext = other.ext;
        env_map = other.env_map;
        full_path = other.full_path;
    }
    return *this;
}
int CGI::start_CGI(std::vector<Database> datab,int value ,int &t_out,std::map<std::string,int> &removing_m, int &child_process, Request &r)
{
    if(r.cgi != 1)
    {
           if(!r.check_error(403))
                    r.full_path = "Errors/error403.html";
                r.status_code = "403 Forbidden";
                t_out = 1;
                return (open(r.full_path.c_str(),O_RDWR,0777));
    }
    if(r.path_cgi.size() > 0)
        r.full_path = r.path_cgi[0];
    if (value == -2)
        return launch_cgi(t_out,removing_m,child_process,r);
     for (unsigned long i ; i < ext.length(); i++)
        ext[i] = std::tolower(ext[i]);   
    if(r.get_url().find("/login") == std::string::npos && !r.get_url().empty())
    {            
        r.full_path = split(r.get_url().substr(1), '?')[0];
    if(r.full_path[r.full_path.length()-1] == 'y')
    {
        ext=  r.full_path.substr( r.full_path.length() - 3);
    }
    else if(r.full_path[r.full_path.length()-1] == 'p')
    {

        ext=  r.full_path.substr( r.full_path.length() -4);
    }

    }
    if(ext == ".py" || ext == ".php")
    {
        if(access(r.full_path.c_str(),F_OK) == 0)
        {


            if(access(r.full_path.c_str(),X_OK) == 0)
            {

                char buffer[20];
                env_map["REQUEST_METHOD"]= r.get_method();
                env_map["PATH_INFO"] = r.full_path;
                env_map["CONTENT_TYPE"]= "text/html";
                env_map["CONTENT_LENGTH"]=  std::string(buffer);
                env_map["REMOTE_ADDR"]= host;
                env_map["SERVER_NAME"]= r.get_server_name();
                env_map["SERVER_PORT"]= port;
                env_map["BODY_CONTENT"] = r.body_content;
                if(r.get_method() == "GET")
                   env_map["QUERY_STRING"] =  r.query_cgi;
                unsigned long int i =0;
                if(r.cooki == 0)
                {

                    for(i = 0; i < datab.size(); i++)
                    {

                        if( !r.cgi_data["firstName"].empty() && (r.cgi_data["firstName"][0] + r.cgi_data["lastName"] == datab[i].username))
                        {
                            
                            r.cgi_data["userName"]= datab[i].username;

                            if (value == 0)
                                r.cgi_data["password"] = datab[i].password;
                            else
                                r.cgi_data["password"] = "1337";
                        }
                        else if(r.cgi_data["userName"] == datab[i].username)
                        {   

                            r.cgi_data["firstName"] = datab[i].firstName;
                            r.cgi_data["lastName"] = datab[i].lastName;
                        }
                    }
                    if(i < datab.size() || datab.empty())
                    {
                        r.cgi_data["password"] = "1337";
                    }
                    else if(r.cgi_data["password"].empty())
                        r.cgi_data["password"] = "1337";

                    env_map.insert(r.cgi_data.begin(), r.cgi_data.end());
                }
                else
                {
                     for(long unsigned int i= 0 ; i < r.sess.size(); i++)
                    {
                        if(r.sess_id == r.sess[i].session)
                        {
                            
                            env_map.insert(r.sess[i].data.begin(), r.sess[i].data.end());
                        }
                        }
                }

                return (launch_cgi(t_out,removing_m,child_process,r));
            }
            else
            {
                if(!r.check_error(500))
                    r.full_path = "Errors/error500.html";
                r.status_code = "500 Internal Server Error";
                t_out = 1;
                return (open(r.full_path.c_str(),O_RDWR,0777));
            }
         
        }
    }
    if(!r.check_error(403))
        r.full_path = "Errors/error403.html";
    r.status_code = "403 Forbidden";
    t_out = 1;
    return (open(r.full_path.c_str(),O_RDWR,0777));
}
void handleTimeout(int signal) {
    std::cerr << RED_COLOR << "CGI script execution timed out (Signal " << signal << ")." << RESET_COLOR<< std::endl;

    std::exit(123);
}
int CGI::launch_cgi(int &t_out,std::map<std::string,int> &removing_m, int &child_process, Request &r)
{
    int status;
    int tmp;
    if(t_out == 0)
    {

        t_out = waitpid(child_process, &status, WNOHANG);
        while ((tmp = waitpid(-1, 0, WNOHANG)) > 0) 
        {
            if(tmp != child_process)
                kill(tmp, SIGTERM);
        
        }
        if(t_out != 0)
        {

            
        if(fd != -1)
            close(fd);
        
        if(child_process > 0)
            kill(child_process,SIGTERM);
       if (WIFSIGNALED(status) || WEXITSTATUS(status) != 0)
       {

            if(WIFEXITED(status))
            {
                close(r.fd_file);
                if(!r.check_error(502))
                    r.full_path = "Errors/error502.html";
                r.fd_file = open(r.full_path.c_str(),O_CREAT| O_RDWR,755);

                r.status_code = "502 Bad Gateway";
            }
            else if(WIFSIGNALED(status) && WEXITSTATUS(status) == 0){
                close(r.fd_file);
                 if(!r.check_error(504))
                    r.full_path = "Errors/error504.html";
                r.fd_file = open(r.full_path.c_str(),O_CREAT| O_RDWR,755);
                r.status_code = "504 Gateway Timeout";
            }
        }

        lseek(r.fd_file, 0, SEEK_SET);
    }
        return (r.fd_file);

    }

    
    char **env = new char *[env_map.size() + 1];
   char **arg = new char *[3]; 

   if(ext == ".py")
   {
        arg[0] = new char[strlen("/usr/bin/python3.10")+ 1];
        strcpy(arg[0], "/usr/bin/python3.10"); 
    }
    else
    {
         arg[0] = new char[strlen("/usr/bin/php8.1")+ 1]; 
        strcpy(arg[0], "/usr/bin/php8.1"); 
    }
   arg[1] = new char[strlen(env_map["PATH_INFO"].c_str()) + 1]; 
    strcpy(arg[1], env_map["PATH_INFO"].c_str());
    arg[2] = NULL;
    int i = 0;
for (std::map<std::string, std::string>::const_iterator it = env_map.begin(); it != env_map.end(); ++it) {
        std::string entryStr = it->first + "=";
        entryStr.append(it->second);
        env[i] = new char[entryStr.length() + 1];
        long unsigned int j;
        for( j= 0; j < entryStr.length(); j++)
            env[i][j] = entryStr[j];
        env[i][j] = '\0';
     
        i++;
    }
    env[i] = NULL;
    std::string filename;
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        std::srand(static_cast<unsigned int>(std::time(NULL)));
    filename = "/tmp/.";
    std::string fileout = "/tmp/.";
    for (int i = 0; i < 15; ++i)
            filename += alphanum[std::rand() % (sizeof(alphanum) - 1)];
    for(int i=0; i< 5 ;++i)
        fileout += alphanum[std::rand() % (sizeof(alphanum) - 1)];
    close(r.fd_file);
    r.fd_file = open(filename.c_str(),O_CREAT| O_RDWR| O_TRUNC,755);
    removing_m[filename] = r.fd_file ;

    fd =  open(fileout.c_str(),O_CREAT| O_RDWR| O_TRUNC,755);  
    write(fd,env_map["BODY_CONTENT"].c_str(),env_map["BODY_CONTENT"].length());
    lseek(fd, 0, SEEK_SET);
    removing_m[fileout] = fd;


    int id = fork();
    if (id == 0) 
    {
        dup2(fd, 0);
        dup2(r.fd_file, 1);
        dup2(r.fd_file, 2); 

        close(r.fd_file);
        
        std::signal(SIGALRM, handleTimeout);
        alarm(15);

        execve(arg[0], arg, env);
        std::cerr << "Execve" << std::endl;
        exit(1);
    }
    else
    {
        child_process = id;
        t_out = waitpid(child_process, &status, WNOHANG);

        while ((tmp = waitpid(-1, 0, WNOHANG)) > 0) {
            if(tmp != child_process)
                kill(tmp, SIGTERM);
        
        }
        if(t_out != 0)
        {
        close(fd);
        if(child_process > 0)
            kill(child_process,SIGTERM);

       if (WIFSIGNALED(status) || WEXITSTATUS(status) != 0)
       {

            close (r.fd_file);
            if(WIFEXITED(status))
            {
                close(r.fd_file);

                 if(!r.check_error(502))
                    r.full_path = "Errors/error502.html";
                r.fd_file = open(r.full_path.c_str(),O_CREAT| O_RDWR,755);
                r.status_code = "502 Bad Gateway";
            }
            else if(WIFSIGNALED(status) && WEXITSTATUS(status) == 0){
                close(r.fd_file);
                 if(!r.check_error(504))
                    r.full_path = "Errors/error504.html";
                r.fd_file = open(r.full_path.c_str(),O_CREAT| O_RDWR,755);
                r.status_code = "504 Gateway Timeout";
            }
        }

                
        lseek(r.fd_file, 0, SEEK_SET);

    }

    }
     
       for (int i = 0; env[i] != NULL; i++) {
            delete[] env[i];
        }
        delete[] env;


delete[] arg[0];
delete[] arg[1];
delete[] arg;
    return (r.fd_file);
}

CGI::~CGI()
{

}
