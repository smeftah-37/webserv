
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include<vector>
#include "Server/Servers.hpp"
#include "Server/ServerTCP.hpp"
#include "Configfile/location.hpp"

int main(int argc, char **argv)
{
    std::string filename;
    if(argc == 2)
        filename = std::string(argv[1]);
    else
    {
        filename = "config_file.conf";
    }
    config_file co = config_file(filename);    Servers servers = Servers(co);
    servers.main_project();
}