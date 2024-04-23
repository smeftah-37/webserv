#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <arpa/inet.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include<vector>
#include<cstring>
#include<cstdlib>
#include<utility>
#include <limits.h>
#include <map>
#include "../Configfile/location.hpp"
#include "../Configfile/config_file.hpp"
#include "../Configfile/server.hpp"
#include <ctime>

#define RESET_COLOR "\033[0m"
#define RED_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"
#define YELLOW_COLOR "\033[33m"
#define BLUE_COLOR "\033[34m"
#define MAGENTA_COLOR "\033[35m"
#define RED_ON_WHITE "\033[31;47m"

#define BUFFER_SIZE 4096
typedef struct Database{
    std::string firstName;
    std::string lastName;
    std::string password;
    std::string username;
}Database;
typedef struct Session {
    std::string domain_name;
    std::string session;
    std::map <std::string, std::string> data;
    std::string expiration; 
    std::string path;
}Session;

class Request
{
    private:
        std::string method;
        int last_count_chunked;
        std::string reserv_chunked;
        std::string http_version;
        size_t max_content;
        std::string url;

        std::string header_line;
        std::vector<location> loc;
        
        std::string error_file;

        std::string request_line;
        std::string header_fields;
        std::string boundary;
        std::string post_method;        
        



    public: 
     std::string status_code;
     int nothing_to_do;       
        size_t body_length;
        int t_out;
        int id_child;
        int  minikamal;
        Database datab;
        int hhh;
        std::string ext;
        std::string query_cgi;
        std::map <std::string,int>removing_file;
        std::vector<std::pair<std::string, std::string> >variables;
        server Server;
        int listning_file;
        std::string full_path;
        std::string session_id;
        std::string session_timer;
        std::string session_path;
        std::string upload_path;
        std::string cookie;
        std::vector<Session> sess;
        std::vector<std::string> path_cgi;
        int check_c;
        Session session_;
        std::string key;
        int cgi;
        int cooki;
        std::string value;
        std::string location_resp;
        std::map<std::string, std::string > cgi_data;
        int fd_file;
       std::string body_content;
        Request(const Request& other);
        void display_time(time_t currentTime);
        void set_hhh(const int &x);

    Request& operator=(const Request& other);
    void handle_delete(const char *path);
    std::string get_pure_url(std::string a);

        char buffer[BUFFER_SIZE + 1];
        std::string filename_boundary;
        int type_json;
        int fileupl;
        
        std::ofstream file;
        int count_chunked;
std::string type_chunked(std::string &chunk);

        size_t read_b;
        std::string sess_id;
        std::map<std::string, std::string> querydata;
std::string help_function(std::string file, std::string l, std::string name);


        std::string chunked_data;
        std::vector<std::string> array_data;
        std::string string_data;
        std::string host;
        Request();
        size_t get_body_length();
        int check_sessionid(std::string session_id);
        bool is_expired(int i,const char* str);
        int parse_cookie(std::string str);
        int check_cookies(int i);
        int set_session();
        int text_plain();
        int json_object();
        int array_object();
        int string_object();
        int extract_data(std::vector<std::string> data );
        int check_data_type();
        int json_data();

        std::string get_server_name();
         std::string get_body_content();
        std::string get_boundary();
        Request(std::string header_str,std::vector<std::pair<server,std::vector<location> > > s, size_t len, std::vector<Session> sessions);
        std::string get_method() const;

        std::string get_url();
        std::string get_http_version() const;
        std::string get_status_code() const;
        int check_error(int err);
        int check_method(std::vector<std::string> methods);
        int form_data(std::string spliter );
        int check_url();
        size_t get_max_content();
        int check_UrlLength();
        int check_Length();
        std::string get_post_method();
        int extract_data_url();
        int open_File();
        time_t chrono_;
        std::string get_split_url();

        int init_method();
       int  handle_post(int fd);
        std::string url_decode();

        int check_resp;
        std::string get_path();


        ~Request();
};


std::vector<std::string> split_word(const std::string& str, const std::string& delimiter);
#endif