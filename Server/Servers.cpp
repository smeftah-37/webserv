#include "Servers.hpp"


#include <algorithm>

Servers::Servers(config_file configfile)
{
    std::list<std::pair<server, std::vector<location> > >::iterator it;
    this->nothing = -1;
    this->nb_serv = 0;
    request_map = new(std::map<int, Request>);
    this->register_ = 0;
    this->login_ = 0;
    this->head = 0;
    search = configfile.serv;
    for (it = configfile.serv.begin(); it != configfile.serv.end(); ++it)
    {
        server serverObj = it->first;
        std::vector<location> locationVector = it->second;
        this->serving.push_back(ServerTCP(serverObj.host, serverObj.listen, locationVector));
        this->nb_serv++;
    }

    ep.ep_fd = epoll_create(1);
    if (ep.ep_fd == -1)
        perror("epoll");
    for (int i = 0; i < nb_serv; i++)
    {
        server s;
        set_socket(serving[i]);
        
    }
}

void Servers::add_to_epoll(int epoll_fd, int socket_fd)
{
      struct epoll_event event;
        event.events = EPOLLIN; 
        event.data.fd = socket_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1) {
            perror("epoll_ctl EPOLL_CTL_ADD");
            close(socket_fd);
        }

}
int check_loop(std::vector<std::pair<std::string, int> > ports ,int port , std::string host)
{
    for (std::vector<std::pair<std::string, int> >::const_iterator it = ports.begin(); it != ports.end(); ++it) 
    {
        if (it->first == host && it->second == port) 
        {
            return(1);
        }
    }
    return 0;
}

int Servers::set_socket(ServerTCP &serv)
{
    int x = -1;
    for (size_t i = 0; i < serv.get_port().size();  i++) 
    {
        if(check_loop(protected_port,serv.get_port()[i],serv.get_host()))
            continue;
        x++;
        std::pair<std::string, int> hostport = std::make_pair(serv.get_host(),serv.get_port()[i]);
        protected_port.push_back(hostport);
        serv.fds_socket.push_back(socket(AF_INET, SOCK_STREAM, 0));
        int reuse = 1;
        if (setsockopt(serv.fds_socket[x], SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
            perror("setsockopt SO_REUSEADDR failed");

        fds_socket.insert(std::make_pair(serv.fds_socket[x], serv.get_host()));

        serv.address.sin_port = htons(serv.get_port()[i]);

        if (bind( serv.fds_socket[x], (struct sockaddr *)(&serv.address), sizeof(serv.address)) == -1) 
        {
            perror("BIND");
        }
        if (listen( serv.fds_socket[x], serv.get_backlog()) == -1) 
        {
            perror("listen");
        }
        ep.ev.events = EPOLLIN;
        ep.ev.data.fd =  serv.fds_socket[x];
        if (epoll_ctl(ep.ep_fd, EPOLL_CTL_ADD,  serv.fds_socket[x], &ep.ev) == -1)
        {
            close(ep.ep_fd);
            close( serv.fds_socket[x]);
            perror("epoll ctl");
        }
    }
    return 0;
}

void Servers::findserver(std::vector<std::pair<server,std::vector<location> > > &s, std::string host)
{
    std::list<std::pair<server, std::vector<location> > >::iterator it;
    server serverObj;
    std::vector<location> locationVector;
    for (it = search.begin(); it != search.end(); ++it)
    {
        serverObj = it->first;
        locationVector = it->second;
        if (serverObj.host == host)
        {
            std::pair<server, std::vector<location> > shost = std::make_pair(serverObj,locationVector);
            s.push_back(shost);
        }
    }
}

void Servers::handle_new_connection(int fd,std::string buffer, std::vector<std::pair<server,std::vector<location> > > &s)
{
    (void)buffer;
    (void)s;
    this->fds_socket[fd] = s[0].first.host;
}

int Servers::handle_client_data(int fd, std::vector<std::pair<server,std::vector<location> > > &s)
{

    signal(SIGPIPE, SIG_IGN);
    ssize_t len;
    std::string a;
    std::string header;

    bool headerComplete = false;
    size_t contentLength = 0;
    size_t headerEnd = 0;


    if((*request_map).find(fd) == (*request_map).end())
    {
      
        len = recv(fd, buffer, BUFFER_SIZE, 0);
        
            if(len > 0)
            {
            buffer[len] = 0;
            a.append(buffer, len);  

            if (!headerComplete) { 
                
                headerEnd = a.find("\r\n\r\n");
                if (headerEnd != std::string::npos) {

                    headerComplete = true;
                    header = a.substr(0, headerEnd);
                    size_t contentLengthPos = header.find("Content-Length: ");

                    if (contentLengthPos != std::string::npos) {
                        contentLength = atoi((header.substr(contentLengthPos + 16)).c_str());
                    }

                }
                else if(a.length() < BUFFER_SIZE)
                {
                    headerComplete = true;
                }
            
            }
            }
            if (headerComplete) 
            {  
                head = 1;
                (*request_map)[fd] = Request(header, s,contentLength,sessions);

                (*request_map)[fd].read_b = a.length() - headerEnd - 4;
                (*request_map)[fd].open_File();

                if(a.length() > headerEnd + 4)
                    (*request_map)[fd].body_content = a.substr(headerEnd + 4);
                if (contentLength == 0 &&(*request_map)[fd].get_post_method().find("Chunked")  ==  std::string::npos)
                {

                    if ((*request_map)[fd].get_method() == "GET")
                    {
                        if((*request_map)[fd].t_out == 0 && (*request_map)[fd].get_status_code() == "200 OK")
                        {

                            int value = -2;
                            (*request_map)[fd].fd_file =  cgi.start_CGI(database,value,(*request_map)[fd].t_out,(*request_map)[fd].removing_file, (*request_map)[fd].id_child,(*request_map)[fd]);
                            if((*request_map)[fd].t_out ==0)
                                return (0);
                        }
                        (*request_map)[fd].extract_data_url();
                        int  value = main_database((*request_map)[fd].cgi_data,fd);
                       
                           if((*request_map)[fd].get_status_code() != "200 OK")
                    {
                         (*request_map)[fd].nothing_to_do = 0;
                            ep.ev.events = EPOLLOUT;
                                ep.ev.data.fd = fd;
                                epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                                (*request_map)[fd].fileupl= -1;
                                (*request_map)[fd].file.close();
                                return (1);
                    }
                        if((*request_map)[fd].full_path.find("/cgi-bin") != std::string::npos)
                        { 
                           cgi = CGI(&(*request_map)[fd]);
                           (*request_map)[fd].fd_file =  cgi.start_CGI(database,value,(*request_map)[fd].t_out,(*request_map)[fd].removing_file, (*request_map)[fd].id_child,(*request_map)[fd]);

                           if((*request_map)[fd].t_out != 0)
                           {
                            if ((*request_map)[fd].id_child > 0 )
                                kill((*request_map)[fd].id_child,SIGTERM);
                            (*request_map)[fd].nothing_to_do = 0;

                                ep.ev.events = EPOLLOUT;
                                ep.ev.data.fd = fd;
                                epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                                return (1);
                           }
                           else
                            return (0);
                        }
                    }
                     else if((*request_map)[fd].get_method() == "DELETE")
                    {

                        if((*request_map)[fd].get_status_code() == "200 OK")
                        {
                            (*request_map)[fd].handle_delete((*request_map)[fd].get_path().c_str());
                            (*request_map)[fd].full_path = "filehtml/delete.html";
                            (*request_map)[fd].fd_file = open("filehtml/delete.html" ,O_RDWR | O_CREAT, 0666);
                        }
                        
                        
                        
                    }
                    
                    (*request_map)[fd].nothing_to_do = 0;
                    ep.ev.events = EPOLLOUT;
                    ep.ev.data.fd = fd;
                    epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);

                    return (1);
                }
                else if((*request_map)[fd].read_b >= contentLength)
                {
                    if((*request_map)[fd].get_status_code() != "200 OK")
                    {
                         (*request_map)[fd].nothing_to_do = 0;
                            ep.ev.events = EPOLLOUT;
                                ep.ev.data.fd = fd;
                                epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                                (*request_map)[fd].fileupl= -1;
                                (*request_map)[fd].file.close();
                                return (1);
                    }
                    if((*request_map)[fd].get_method() == "POST")
                    {
                        int check = (*request_map)[fd].handle_post(fd);
                        if (check == 1)
                        {
                             if((*request_map)[fd].t_out == 0)
                            {
                                int value = -2;
                                (*request_map)[fd].fd_file =  cgi.start_CGI(database,value,(*request_map)[fd].t_out,(*request_map)[fd].removing_file, (*request_map)[fd].id_child,(*request_map)[fd]);
                                if((*request_map)[fd].t_out ==0)
                                    return (0);
                            }
                                int value = main_database((*request_map)[fd].cgi_data,fd);

                              if((*request_map)[fd].full_path.find("/cgi-bin") != std::string::npos)
                            {

                           cgi = CGI(&(*request_map)[fd]);
                           (*request_map)[fd].fd_file =  cgi.start_CGI(database,value,(*request_map)[fd].t_out,(*request_map)[fd].removing_file, (*request_map)[fd].id_child,(*request_map)[fd]);
                            if((*request_map)[fd].t_out != 0)
                           {

                                 if ((*request_map)[fd].id_child > 0 )
                                    kill((*request_map)[fd].id_child,SIGTERM);
                                (*request_map)[fd].nothing_to_do = 0;
                                ep.ev.events = EPOLLOUT;
                                ep.ev.data.fd = fd;
                                epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                                return (1);
                           }
                           else
                            return (0);
                            }
                            (*request_map)[fd].nothing_to_do = 0;
                            ep.ev.events = EPOLLOUT;
                                ep.ev.data.fd = fd;
                                epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                                (*request_map)[fd].fileupl= -1;
                                (*request_map)[fd].file.close();
                                return (1);
                        }
                        else if (check == -1)
                        {
                            (*request_map)[fd].listning_file = 0;
                            if(!(*request_map)[fd].check_error(204))
                                (*request_map)[fd].full_path = "Errors/error204.html";
                             (*request_map)[fd].fd_file = open((*request_map)[fd].full_path.c_str(),O_CREAT| O_RDWR,755);
                            (*request_map)[fd].status_code = "204 No Content";
                            (*request_map)[fd].nothing_to_do = 0;
                            ep.ev.events = EPOLLOUT;
                            ep.ev.data.fd = fd;
                            epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                            return 1;
                        }
                    }
                    else if((*request_map)[fd].get_method() == "DELETE")
                    {
                       if((*request_map)[fd].get_status_code() == "200 OK")
                    {
                            (*request_map)[fd].handle_delete((*request_map)[fd].get_path().c_str());
                            (*request_map)[fd].full_path = "filehtml/delete.html";
                            (*request_map)[fd].fd_file = open("filehtml/delete.html" ,O_RDWR | O_CREAT, 0666);
                     }
                        
                    (*request_map)[fd].nothing_to_do = 0;
                        
                        ep.ev.events = EPOLLOUT;
                        ep.ev.data.fd = fd;
                        epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                        return (1);
                        
                    }
                    else
                    {
                          (*request_map)[fd].nothing_to_do = 0;
                        
                        ep.ev.events = EPOLLOUT;
                        ep.ev.data.fd = fd;
                        epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                        return (1);
                        
                    }
                    return (0);
                            
                }
                }
                
            }

    else
    {
    if ((*request_map)[fd].get_method() == "GET")
    {
           if((*request_map)[fd].get_status_code() != "200 OK")
            {
                    (*request_map)[fd].nothing_to_do = 0;
                    ep.ev.events = EPOLLOUT;
                        ep.ev.data.fd = fd;
                        epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                        (*request_map)[fd].fileupl= -1;
                        (*request_map)[fd].file.close();
                        return (1);
            }
        if((*request_map)[fd].t_out == 0)
        {
            int value = -2;
            (*request_map)[fd].fd_file =  cgi.start_CGI(database,value,(*request_map)[fd].t_out,(*request_map)[fd].removing_file, (*request_map)[fd].id_child,(*request_map)[fd]);
            if((*request_map)[fd].t_out ==0)
                return (0);
        }
        (*request_map)[fd].extract_data_url();
          main_database((*request_map)[fd].cgi_data,fd);

         if((*request_map)[fd].full_path.find("/cgi-bin") != std::string::npos)
        {
            if((*request_map)[fd].t_out != 0)
            {
             if ((*request_map)[fd].id_child > 0 )
                kill((*request_map)[fd].id_child,SIGTERM);
            (*request_map)[fd].nothing_to_do = 0;
                ep.ev.events = EPOLLOUT;
                ep.ev.data.fd = fd;
                epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                return (1);
            }
            else
                return (0);
            (*request_map)[fd].nothing_to_do = 0;
            return (1);
         }
           (*request_map)[fd].nothing_to_do = 0;
                            ep.ev.events = EPOLLOUT;
                                ep.ev.data.fd = fd;
                                epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                                (*request_map)[fd].fileupl= -1;
                                (*request_map)[fd].file.close();
                                return (1);
    }
        else if((*request_map)[fd].get_method() == "POST")
        { 

            if((*request_map)[fd].get_status_code() != "200 OK")
                {

                        (*request_map)[fd].nothing_to_do = 0;
                        ep.ev.events = EPOLLOUT;
                            ep.ev.data.fd = fd;
                            epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                            (*request_map)[fd].fileupl= -1;
                            (*request_map)[fd].file.close();
                            return (1);
                }
            if((*request_map)[fd].t_out == 0)
            {
                int value = -2;
                (*request_map)[fd].fd_file =  cgi.start_CGI(database,value,(*request_map)[fd].t_out,(*request_map)[fd].removing_file, (*request_map)[fd].id_child,(*request_map)[fd]);
                if((*request_map)[fd].t_out ==0 )
                    return (0);
                else
                {
                   (*request_map)[fd].nothing_to_do = 0;
                    ep.ev.events = EPOLLOUT;
                    ep.ev.data.fd = fd;
                    epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                    (*request_map)[fd].fileupl= -1;
                    (*request_map)[fd].file.close();
                    return (1);
                }

            }
            int check = (*request_map)[fd].handle_post(fd);
            
            if (check == 1)
            {
               
                   int value = main_database((*request_map)[fd].cgi_data,fd);

            
                 if((*request_map)[fd].full_path.find("/cgi-bin") != std::string::npos)
                {

                    cgi = CGI(&(*request_map)[fd]);

                           (*request_map)[fd].fd_file =  cgi.start_CGI(database,value,(*request_map)[fd].t_out,(*request_map)[fd].removing_file, (*request_map)[fd].id_child,(*request_map)[fd]);
                         if((*request_map)[fd].t_out != 0)
                           {
                                 if ((*request_map)[fd].id_child > 0 )
                                    kill((*request_map)[fd].id_child,SIGTERM);
                                (*request_map)[fd].nothing_to_do = 0;
                                ep.ev.events = EPOLLOUT;
                                ep.ev.data.fd = fd;
                                epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                                return (1);
                           }
                           else
                            return (0);
                }
                    (*request_map)[fd].nothing_to_do = 0;
                    ep.ev.events = EPOLLOUT;
                    ep.ev.data.fd = fd;
                    epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                    (*request_map)[fd].fileupl= -1;
                    (*request_map)[fd].file.close();
                    return (1);
            }
            else if(check == -1)
            {
                (*request_map)[fd].listning_file = 0;
                if(!(*request_map)[fd].check_error(204))
                    (*request_map)[fd].full_path = "Errors/error204.html";
                 (*request_map)[fd].fd_file = open((*request_map)[fd].full_path.c_str(),O_CREAT| O_RDWR,755);
                (*request_map)[fd].status_code = "204 No Content";
                (*request_map)[fd].nothing_to_do = 0;
                ep.ev.events = EPOLLOUT;
                ep.ev.data.fd = fd;
                epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                return 1;
            }
            return (0);
                
        }  
         else if((*request_map)[fd].get_method() == "DELETE")
                    {
                      if((*request_map)[fd].get_status_code() == "200 OK")
                        {
                            (*request_map)[fd].handle_delete((*request_map)[fd].get_path().c_str());
                            (*request_map)[fd].full_path = "filehtml/delete.html";
                            (*request_map)[fd].fd_file = open("filehtml/delete.html" ,O_RDWR | O_CREAT, 0666);
                       }
                        
                    (
                        *request_map)[fd].nothing_to_do = 0;
                        
                        
                        ep.ev.events = EPOLLOUT;
                        ep.ev.data.fd = fd;
                        epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                        return (1);
                        
                    }
                else
                {
                      (*request_map)[fd].nothing_to_do = 0;
                        
                        ep.ev.events = EPOLLOUT;
                        ep.ev.data.fd = fd;
                        epoll_ctl(ep.ep_fd, EPOLL_CTL_MOD, fd, &ep.ev);
                        return (1);
                        
                }

   
    }
    return (0);
}


void Servers::handle_http_request(int fd,std::vector<std::pair<server,std::vector<location> > > &s)
{
   
    (void)fd;
    (void)s;
    signal(SIGPIPE, SIG_IGN);
  
}
void Servers::handleTimeouts(int fd, int j) 
{
   time_t now_ = time(NULL);
    if((now_ - lastActivityTime[fd] ) >=TIMEOUT_SECONDS)
    {
          if((*request_map)[fd].removing_file.size() > 0)
            {
                for (std::map<std::string,int>::iterator it = (*request_map)[fd].removing_file.begin(); it != (*request_map)[fd].removing_file.end(); ++it) 
                {
                    close(it->second);

                    std::remove(it->first.c_str());


                }
            
            }
        epoll_ctl(ep.ep_fd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        serving[j].cls_socket.erase(std::find(serving[j].cls_socket.begin(), serving[j].cls_socket.end(),fd));
        lastActivityTime.erase(fd);
        display_time(now_);
        std::cout << RED_COLOR << "15 seconds passed, without any request from Client fd :" << fd << " | CLOSED" << RESET_COLOR <<std::endl; 
        for (std::map<int, int>::iterator it = policy.begin(); it != policy.end(); ++it)  
        {
            if (it->first == fd)
            {
                policy.erase(it);
                break;  
            }
        } 
    }
}
void Servers::display_time(time_t currentTime)
{
    struct tm *localTime = localtime(&currentTime);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S: ", localTime);
    std::cout << MAGENTA_COLOR << buffer << RESET_COLOR;
}
int Servers::check_last(int first, int second)
{
    for (std::map<int ,int>::const_iterator it = policy.begin(); it != policy.end(); ++it) 
    {
        if(it->first == first)
        {
            if(it->second == second)
                return(1);
        }
    }
    return(0);
}
void welcome()
{
    std::cout << RED_COLOR;

 std::cout <<
"          _KIKI_                      \n"
"        dGGGGMMb     ,\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\".\n"
"       @p~qp~~qMb    |          WELCOME TO OUR WEBSERV        |\n"
"       M|(@||@)M|   _|          by : kremidi & smeftah-       |\n"
"       @,----.JM| -'_;........................................'\n"
"      JS^\\__/  qKL\n"
"     dZP        qKRb\n"
"    dZP          qKKb\n"
"   fZP   |1337|   SMMb\n"
"   HZM            MMMM\n"
"   FqM            MMMM\n"
" __| \".        |\\dS\"qML\n"
" |    `.       | `' \\Zq\n"
"_)      \\.\\.___.,|     .'\n"
"\\____   )KIKIYA|   .'\n"
"     `-'       `--'    )";
    std::cout << RESET_COLOR << std::endl;
}
int Servers::checkinho(int fd)
{
    std::map<int, int>::iterator it;
    for (it = policy.begin(); it != policy.end(); ++it)
    {
        if (it->first == fd)
            return(1);
    }
    return(0);
}
int Servers::main_project() {
   int check_c = -1;
    welcome();
    Response response;
    std::vector<std::pair<server,std::vector<location> > > s;

    while (1) {
        int num_events = epoll_wait(ep.ep_fd, ep.events, 1, -1);
        if (num_events == -1) 
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        else if(num_events == 0)
        {
            display_time(time(NULL));
            std::cout << RED_COLOR << "No activity for " << TIMEOUT_SECONDS << " seconds. Closing idle connections." << RESET_COLOR << std::endl;
            continue;
        }

        for (int i = 0; i < num_events; ++i)
        {
            for (int j = 0; j < nb_serv; j++)
            {
                if (std::find(serving[j].fds_socket.begin(), serving[j].fds_socket.end(), ep.events[i].data.fd) != serving[j].fds_socket.end())
                {
                    s.clear();
                    findserver(s, fds_socket[ep.events[i].data.fd]);

                    int client_socket = accept(ep.events[i].data.fd, NULL, NULL);
                    if (client_socket == -1) 
                    {
                        continue;
                    }
                    if (fcntl(client_socket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1) {
                            perror("fcntl");
                            continue; // Handle the error
                    }
                    else{
                         lastActivityTime[client_socket] = time(NULL);
                         display_time(lastActivityTime[client_socket]);
                         std::cout << GREEN_COLOR << "New Connection Accepted | " << client_socket << RESET_COLOR << std::endl;
                         this->policy[client_socket] = nothing;
                    }
                    ep.ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
                    ep.ev.data.fd = client_socket;
                    serving[j].cls_socket.push_back(client_socket);

                    if (epoll_ctl(ep.ep_fd, EPOLL_CTL_ADD, client_socket, &ep.ev) == -1)
                    {
                        perror("epoll_ctl");
                        close(client_socket);
                    }
                }
            }
            
            for (int j = 0; j < nb_serv; j++)
            {
                if (std::find(serving[j].cls_socket.begin(), serving[j].cls_socket.end(), ep.events[i].data.fd) != serving[j].cls_socket.end())
                {
                     if(ep.events[i].events & EPOLLERR || ep.events[i].events & EPOLLHUP || ep.events[i].events & EPOLLRDHUP)
                    {
                         if((*request_map)[ep.events[i].data.fd].removing_file.size() > 0)
                            {
                                for (std::map<std::string,int>::iterator it = (*request_map)[ep.events[i].data.fd].removing_file.begin(); it != (*request_map)[ep.events[i].data.fd].removing_file.end(); ++it) 
                                {
                                    close(it->second);

                                    std::remove(it->first.c_str());
                    
                                }
                            
                            }
                        if ((*request_map)[ep.events[i].data.fd].id_child > 0 )
                            kill((*request_map)[ep.events[i].data.fd].id_child,SIGTERM);
                        epoll_ctl(ep.ep_fd, EPOLL_CTL_DEL, ep.events[i].data.fd, NULL);
                        close(ep.events[i].data.fd);
                        std::map<int, Request>::iterator it = (*request_map).find(ep.events[i].data.fd);
                        if (it != (*request_map).end())
                            (*request_map).erase(it);
                        serving[j].cls_socket.erase(std::find(serving[j].cls_socket.begin(), serving[j].cls_socket.end(), ep.events[i].data.fd));
                    }
                    else if (ep.events[i].events & EPOLLIN  || (checkinho(ep.events[i].data.fd) && check_last(ep.events[i].data.fd,-1)))
                    {
                   
                        if(!head)
                            handleTimeouts(ep.events[i].data.fd,j);
                        if (handle_client_data(ep.events[i].data.fd, s))
                        {
                            head = 0;
                            for (std::map<int, int>::iterator it = policy.begin(); it != policy.end(); ++it)
                            {
                                if (it->first == ep.events[i].data.fd)
                                {
                                    it->second = (*request_map)[ep.events[i].data.fd].nothing_to_do;
                                    nothing = (*request_map)[ep.events[i].data.fd].nothing_to_do;
                                    break;
                                }
                            }
                            if((*request_map)[ep.events[i].data.fd].get_method() == "POST"  &&(*request_map)[ep.events[i].data.fd].status_code == "200 OK")
                                (*request_map)[ep.events[i].data.fd].status_code = "201 Created";
                            response = Response((*request_map), ep.events[i].data.fd, &check_c);
                        }
                        continue;
                    }
                    else if (ep.events[i].events & EPOLLOUT &&  checkinho(ep.events[i].data.fd) && check_last(ep.events[i].data.fd,(*request_map)[ep.events[i].data.fd].nothing_to_do) && (*request_map)[ep.events[i].data.fd].nothing_to_do == 0) 
                    {

                        if (response.get_response(ep.events[i].data.fd, fds_socket, &check_c, (*request_map), sessions, database)) 
                        {

                            if ((*request_map)[ep.events[i].data.fd].check_c == 1)
                            {
                                sessions.push_back((*request_map)[ep.events[i].data.fd].session_);
                            } 
                            if((*request_map)[ep.events[i].data.fd].removing_file.size() > 0)
                            {
                                for (std::map<std::string,int>::iterator it = (*request_map)[ep.events[i].data.fd].removing_file.begin(); it != (*request_map)[ep.events[i].data.fd].removing_file.end(); ++it) 
                                {
                                    close(it->second);

                                    std::remove(it->first.c_str());
                    
                                }
                            
                            }
                            epoll_ctl(ep.ep_fd, EPOLL_CTL_DEL, ep.events[i].data.fd, NULL);
                            close(ep.events[i].data.fd);
                            display_time(time(NULL));
                           
                            std::cout << RED_COLOR << "The response is send && The connection with Client fd socket :" <<  ep.events[i].data.fd << " is closed" << RESET_COLOR << std::endl;
                            std::map<int, Request>::iterator it = (*request_map).find(ep.events[i].data.fd);
                            close((*request_map)[ep.events[i].data.fd].fd_file);
                            if (it != (*request_map).end())
                                (*request_map).erase(it);
                            serving[j].cls_socket.erase(std::find(serving[j].cls_socket.begin(), serving[j].cls_socket.end(), ep.events[i].data.fd));
                            for (std::map<int, int>::iterator it = policy.begin(); it != policy.end(); ++it)
                            {
                                if (it->first == ep.events[i].data.fd)
                                {
                                    policy.erase(it);
                                    break;
                                }
                            }
                             nothing = -1;
                             
                        }
                    }
                    else
                    {
                            handleTimeouts(ep.events[i].data.fd,j);
                    }
                }

            }
        }
        
    }
    return 0;
}

std::map<int, std::string> Servers::get_fd_socket()
{
    return (this->fds_socket);
}

std::vector<ServerTCP> Servers::get_all_servers()
{
    return (this->serving);
}

Servers::~Servers()
{
    delete request_map;
    close(epoll_fd);
}