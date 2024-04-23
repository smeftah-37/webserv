#include "ServerTCP.hpp"

ServerTCP::ServerTCP(std::string host, std::vector<int> ports, std::vector<location> loc)
{
    memset(&this->address, 0, sizeof(this->address));
    this->host = host;
    this->backlog = 5;
    this->ports = ports;
    this->domain = AF_INET;
    this->service = SOCK_STREAM;
    this->protocol = 0;
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = inet_addr(host.c_str()); 
    this->address.sin_port = htons(ports[0]);
    
    this->loc = loc;

}

std::vector<int> ServerTCP::get_port()
{
    return(this->ports);
}

int ServerTCP::get_porting()
{
    return(this->port);
}

int ServerTCP::get_fd_socket(int i)
{
    return(this->fds_socket[i]);
}

void ServerTCP::set_port(int i)
{
    this->port = this->ports[i];
}

struct sockaddr_in *ServerTCP::get_address() 
{
    return (&(this->address));
}

int ServerTCP::get_backlog()
{
    return(this->backlog);
}

int ServerTCP::get_protocol()
{
    return(this->protocol);
}

int ServerTCP::get_service()
{
    return(this->service);
}


int ServerTCP::get_domain()
{
    return(this->domain);
}


std::string ServerTCP::get_host()
{
    return(this->host);
}

ServerTCP::~ServerTCP()
{
}