#include "Request.hpp"
#include <cerrno>

Request::Request()
{
    hhh=0;
    check_resp = 0;
    nothing_to_do = 1;
    type_json = -1;
    fileupl = -1;
    cgi = 0;
    t_out = 1;
    id_child = -1;
    check_c = -1;
    minikamal = 0;
    count_chunked = 0;
    fd_file =  -1;
    this->cooki= 0;
}
std::vector<std::string> split_word(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        std::string kikism = str.substr(start, end - start);
        if(!kikism.empty())
            tokens.push_back(kikism);
        start = end + delimiter.length();
    }

    tokens.push_back(str.substr(start));

    return tokens;
}

Request::Request(const Request& other)
{
    *this = other;
}


Request& Request::operator=(const Request& other)
{
    if (this != &other) {
        method = other.method;
        nothing_to_do=other.nothing_to_do;
        http_version = other.http_version;
        max_content = other.max_content;
        url = other.url;
        cooki = other.cooki;
        Server = other.Server;
        chrono_ = other.chrono_;
        loc = other.loc;
        removing_file = other.removing_file;
        status_code = other.status_code;
        full_path = other.full_path;
        error_file = other.error_file;
        variables = other.variables;
        location_resp = other.location_resp;
        request_line = other.request_line;
        header_fields = other.header_fields;
        header_line = other.header_line;
        boundary = other.boundary;
        post_method = other.post_method;
        querydata = other.querydata;
        cgi_data = other.cgi_data;
        session_ = other.session_;
        listning_file = other.listning_file;
        t_out = other.t_out;
        minikamal = other.minikamal;
        cgi = other.cgi;
        id_child = other.id_child;
        check_c = other.check_c;
        sess = other.sess;
        datab = other.datab;
        body_content = other.body_content;
        chunked_data = other.chunked_data;
        body_length = other.body_length;
        read_b = other.read_b;
        fileupl = other.fileupl;
        path_cgi = other.path_cgi;
        fd_file = other.fd_file;
        sess_id = other.sess_id;
        hhh=other.hhh;
       check_resp = other.check_resp;
       count_chunked = other.count_chunked;
       ext =  other.ext;
    }
    return *this;
}

void Request::display_time(time_t currentTime)
{
    struct tm *localTime = localtime(&currentTime);

    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S: ", localTime);
    std::cout << MAGENTA_COLOR << buffer << RESET_COLOR;

}

std::string similarity(const std::string& originalString, const std::vector<std::string>& substrings) {
    std::string mostSimilarSubstring;
    size_t maxCommonPrefixLength = 0;

    for (size_t j = 0; j < substrings.size(); ++j) {
        const std::string& substring = substrings[j];

        size_t commonPrefixLength = 0;
        size_t minLength = std::min(originalString.length(), substring.length());

        for (size_t i = 0; i < minLength; ++i) {
            if (originalString[i] == substring[i]) {
                commonPrefixLength++;
            } else {
                break;
            }
        }

        if (commonPrefixLength > maxCommonPrefixLength) {
            maxCommonPrefixLength = commonPrefixLength;
            mostSimilarSubstring = substring;
        }
    }

    return mostSimilarSubstring;
}

Request::Request(std::string header_str,std::vector<std::pair<server,std::vector<location> > > s, size_t len, std::vector<Session> sessions)
{
  
    check_resp = 0;
    nothing_to_do = 1;
    type_json = -1;
    fileupl = -1;
    listning_file = 0;
    cgi = 0;
    sess = sessions;
    t_out = 1;
    id_child = -1;
    check_c = -1;
    minikamal = 0;
    count_chunked = 0;
    chrono_=time(NULL);
    fd_file =  -1;
    hhh=0;
    this->cooki= 0;
    this->request_line = header_str;
    std::vector<std::string> parser ;
    this->header_line = header_str;
    parser = split(this->request_line,'\n');
    if(parser.size() > 0)
        this->request_line = parser[0];
    std::vector<std::string> spliter;
    for(unsigned long i = 1; i < parser.size() ; i++ )
    {
        spliter = split(parser[i],':');
        
        if(spliter.size() > 2)
        {
            for (unsigned long j = 2; j < spliter.size(); j++)
            {
                spliter[1] += ":";
                spliter[1] += spliter[j];
            }
        }

        if(i < parser.size() - 1)
            spliter[1].erase(spliter[1].end() - 1);
         this->variables.push_back(std::make_pair(spliter[0], spliter[1].substr(1)));
    }
    std::string host_;
    std::string port_;
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = variables.begin(); it != variables.end(); ++it) 
    {
        if(it->first == "Host")
        {
            std::vector<std::string> parser = split(it->second,':');
            if(parser.size() > 1)
            {
                host_ = parser[0];
                if(host_ == "localhost")
                    host_ = "127.0.0.1";

                port_ = parser[1];
            }
        }
    }
    parser = split(this->request_line,' ');
    if(parser.size() > 2)
    {
        this->method = parser[0];
        this->url = parser[1];
        this->http_version = parser[2];
        this->http_version.erase(this->http_version.end() - 1);
    }
     std::map<server, std::string>similar;
     std::vector<std::string> sim;
    for (size_t i = 0; i < s.size(); ++i) 
    {
        server s_ = s[i].first;
        std::vector<location> location_ = s[i].second;
        if(host_ == s[i].first.host || host_ == s[i].first.server_name[0])
        {
            if(host_ == s[i].first.server_name[0])
                host_ = s[i].first.host;
        for(unsigned long j = 0; j < location_.size() ; j++)
        {
                if (location_[j].location_match == "/" && get_url() == "/")
                {
                    this->Server = s_;

                    this->loc = location_;
                    goto exit_loop;
                }
                long unsigned int x = this->url.find(location_[j].location_match);
                if(x != std::string::npos)
                {
                    this->Server = s_;

                    this->loc = location_;
                    similar[s_]=location_[j].location_match;
                    sim.push_back(location_[j].location_match);
                }
        }
        }
        if(loc.size() == 0 && !host_.empty())
        {
            this->Server =s_;
        }
    }
    exit_loop:
    if(similar.size() > 1)
    {
        std::string check = similarity(this->url,sim);
         for (std::map<server, std::string>::const_iterator it = similar.begin(); it != similar.end(); ++it) 
        {

            if(it->second == check)
            {
                for (size_t i = 0; i < s.size(); ++i) 
                {
                    server s_1 = s[i].first;
                    std::vector<location> location_2 = s[i].second;
                    if(it->first.server_name == s_1.server_name)
                    {
                        this->loc = location_2;
                        goto exit_loop2;
                    }

                } 
            }

        }
    }
    exit_loop2 :
    body_length = len;

}
std::string Request::get_status_code() const
{
    return this->status_code;
}

std::string Request::get_body_content()
{
    return this->body_content;
}
std::string Request::get_method() const
{
    return(this->method);
}
size_t Request::get_max_content()
{
    return (max_content);
}
void Request::handle_delete(const char *path) {
    DIR *directory;
    struct dirent *entry;
    struct stat statbuf;

    if ((directory = opendir(path)) == NULL) {
        std::vector<std::string> safae = split(path,'/');
        std::string temp = safae[safae.size() - 1];
        std::string hh = path;
        temp = hh.substr(0,hh.length() - temp.length());
        temp += ".tempfile";
        if(rename(path,temp.c_str())!=0)
            std::cerr << "ERROR RENAME" << std::endl;
         if (remove(temp.c_str()) != 0)
            perror("Failed to delete file");
        
        return;
    }

    while ((entry = readdir(directory)) != NULL) {
        char entryPath[PATH_MAX];
        snprintf(entryPath, PATH_MAX, "%s/%s", path, entry->d_name);

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (lstat(entryPath, &statbuf) == -1) {
            perror("Failed to get file status");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            handle_delete(entryPath); 
        } else {
            if (remove(entryPath) != 0) {
                perror("Failed to delete file");
            }
        }
    }
    
    closedir(directory);

    std::string path_re = path;
    if(path_re[path_re.length() - 2] == '/' &&path_re[path_re.length() - 1] == '.')
        path_re[path_re.length() - 1] = 0;
    if (remove(path_re.c_str()) != 0) {
        perror("Failed to delete directory");
    }

}
std::string Request::url_decode() 
{
    std::string decoded;
    std::string str =  this->url;
    unsigned long i = 0;
    while(url.length() > 1 && url[url.length()-1] == '/')
    {
        url.erase(url.length()-1);
        hhh=1;

    }

    while (i < url.length() -1  && url[i] == '/' &&  url[i + 1] == '/')
    {
        url = url.substr(1);
    }
    for (std::size_t i = 0; i < str.length(); ++i) 
    {
        if (str[i] == '%' && i + 2 < str.length()) {
            int hex_value;
            std::istringstream hex_stream(str.substr(i + 1, 2));
            hex_stream >> std::hex >> hex_value;
            decoded += static_cast<char>(hex_value);
            i += 2;
        } 
        else if (str[i] == '+') 
        {
            decoded += ' ';
        } 
        else {
            decoded += str[i];
        }
    }
    return decoded;
}


std::string Request::get_url()
{
    std::string str = url_decode();
    return(str);
}

std::string Request::get_http_version() const
{
    return(this->http_version);
}

int Request::check_error(int err)
{

 std::map<int, std::string>::iterator it = Server.error.find(err);
    if (it != Server.error.end()) 
    {
        this->full_path = Server.error[err];
        return(1);
    }
        
    return(0);
}


std::string Request::get_post_method()
{
    this->post_method = "Binary/Row";
     std::vector<std::pair<std::string, std::string> >::iterator it;

    for (unsigned long i = 0; i < variables.size(); i++)
    {
        if(variables.at(i).first == "Transfer-Encoding")
        {
            if(variables.at(i).second == "chunked")
            {
                if(this->post_method == "Binary/Row")
                    this->post_method = "Chunked";
                else
                    this->post_method = "Chunked/Boundary";
            }
        }
        else if (variables.at(i).first == "Content-Type")
        {
            if(variables.at(i).second.find("boundary") != std::string::npos)
            {
                size_t j = (variables.at(i).second.find("boundary")) + 9;


                boundary = variables.at(i).second.substr(j,variables.at(i).second.length()-j);

       
                if(this->post_method == "Binary/Row")
                    this->post_method = "Boundary";
                else
                    this->post_method = "Chunked/Boundary";
            }
                 
        }
    }
    return this->post_method;

}

int Request::check_Length()
{
     for (unsigned long i = 0; i < variables.size(); i++)
    {
        if(variables.at(i).first == "Content-Length")
        {
            if((unsigned long)(atoi(variables.at(i).second.c_str())) < Server.max_body)
            {
                if(check_error(413))
                {
                    body_length = atoi(variables.at(i).second.c_str());
                    return 1;
                }
                else
                {
                    this->full_path = "Errors/error413.html";
                }
                this->status_code = "413 Request Entity Too Large";
            }

        }
        return 0;
    }
    return (1);
}

int Request::check_method(std::vector<std::string> methods)
{
    if(!methods.empty())
    {
        if(check_error(400))
            return 1;
        else
            this->full_path = "Errors/error400.html";
        this->status_code = "400 Bad Request";
    }
    for(unsigned long i = 0 ; i < methods.size() ; i++)
    {
        if(this->method == methods[i])
        {
            this->status_code = "200 OK";
            return(0);
        }
    }
    if(this->method == "GET" || this->method == "POST" || this->method == "DELETE")
    {
        if(check_error(405))
            return 1;
        else
            this->full_path = "Errors/error405.html";
        this->status_code = "405 Method Not Allowed";
    }
    else if(this->method == "HEAD" || this->method == "PUT" || this->method == "CONNECT" || this->method == "OPTIONS" || this->method == "TRACE" || this->method == "PATCH")
    {
        if(check_error(501))
             return 1;
        else
            this->full_path = "Errors/error501.html";
        this->status_code = "501 Not Implemented";
    }
    else
    {
        if(check_error(400))
            return 1;
        else
            this->full_path = "Errors/error400.html";
        this->status_code = "400 Bad Request";
    }
    return(1);
}

std::string Request::get_boundary()
{
    return boundary;
}
int Request::check_UrlLength()
{
    return(this->get_url().length() > 2048 );
}
int check_file(const char * path)
{  struct stat fileInfo;

    if (stat(path, &fileInfo) != 0) {
        return false;
    }

    if(S_ISREG(fileInfo.st_mode))
        return (1);
    else if(S_ISDIR(fileInfo.st_mode))
        return (2);
    return (0);
}   
std::string Request::get_pure_url(std::string a)
{
    char resolvedPath[PATH_MAX];
    if (realpath(a.c_str(), resolvedPath) != 0)
    {
        return std::string(resolvedPath);
    }

    return ("");
}
std::string Request::get_split_url()
{
    std::string sstr = get_url();
    if(split(sstr,'?').size() > 0)
        return (split(sstr,'?')[0]);
    return "";
}
void Request::set_hhh(const int &x)
{
    this->hhh = x;
}
int Request::check_url()
{
    std::string path;

    std::string kamal;
    unsigned long i = 0;
  
    std::string url =get_url();
    if(split(url,'?').size() > 0)
        url = split(url,'?')[0];
    int kaynaSiege = 0;
   
    for( i = 0; i < loc.size(); i++)
    {
        if(url == loc[i].location_match)
        {
            break;
        }
    }
    if(i == loc.size() && url != "/signup" && url != "/log")
    {
        i = url.find("/");

        if(i != std::string::npos)
        {
            kamal = url.substr(i + 1, url.length()- i);
            if(hhh)
            {
                kamal.append("/");
            }
            url = url.substr(0,i + 1);
        }

    }
    for(unsigned long i = 0; i < loc.size(); i++)
    {
        if(url == loc[i].location_match)
        {
            if(loc[i].upload.empty() && !Server.upload.empty())
                upload_path= Server.upload;
            else if(loc[i].upload.empty())
               upload_path = Server.root + "/login/home/files";
            else
                upload_path = loc[i].upload;

            if(!loc[i].ret.empty())
            {
                location_resp = loc[i].ret;
                status_code = "301 Moved Permanently";
                return 1;

            }

            if(body_length > Server.max_body)
            {

                if(check_error(413))
                    return 1;
                else
                {
                    this->full_path = "Errors/error413.html";
                    this->status_code = "413 Request Entity Too Large";
                }
                break;

            }
            if(loc[i].cgi_path.size() >0)
            {
                cgi = 1;
                path_cgi = loc[i].cgi_path;
                if(loc[i].ext_cgi.size() > 0)
                    ext = loc[i].ext_cgi[0];
            }
            else if(!loc[i].status_cgi.empty() && loc[i].status_cgi == "on")
            {
                cgi = 1;
            }
            if(check_method(loc[i].methods))
            {
                return 1;
            }
            if(!loc[i].root.empty())
            {

                if(check_UrlLength())
                {
                    if(check_error(414))
                        return 1;
                    else
                        this->full_path = "Errors/error414.html";
                         this->status_code = "414 Request-URI Too Long";
                         break;
                }
                else
                {
                    if(!loc[i].auto_index.empty() &&  loc[i].auto_index == "on")
                    {
                        full_path= loc[i].root + "/";
                    }
                    else
                    {
                         if(check_error(403))
                            return 1;
                        this->full_path = "Errors/error403.html";
                        this->status_code = "403 Forbidden";
                        break;
                    }
                        
                    if(!kamal.empty())
                    {
                        full_path += kamal;
                        if(!get_pure_url(full_path).empty())
                        {
                            if(get_pure_url(full_path).find(loc[i].root) == std::string::npos)
                            {
                                status_code = "403 Forbidden";
                                if(!check_error(403))
                                    full_path = "Errors/error403.html";
                                break;
                            } 
                            if(!check_file(get_pure_url(full_path).c_str()) || (check_file(get_pure_url(full_path).c_str()) == 1 && hhh))
                            {
                                if(!check_error(404))
                                    this->full_path = "Errors/error404.html";
                                this->status_code = "404 Not Found";
                                break;
                            }
                            else if(check_file(get_pure_url(full_path).c_str()) == 2 && get_method() != "DELETE")
                            {
                                listning_file = 1;
                                return 1;
                            }
                        }

                     
                    }
                    else if (loc[i].index.size() != 0)
                        full_path += ("/"  + loc[i].index[0]);
                    else if(get_method() != "DELETE")
                    {
                        full_path = Server.root + "/";
                        if(Server.index.size() > 0)
                            full_path +=Server.index[0];
                        else
                            listning_file = 1;

                    }
                    else
                    {
                        if(!check_error(403))
                            full_path = "Errors/error403.html";
                        status_code = "403 Forbidden";
                    }
                }
            }
            else
            {
                if(check_UrlLength())
                {
                    if(check_error(414))
                        return 1;
                    else
                        this->full_path = "Errors/error414.html";
                    this->status_code = "414 Request-URI Too Long";
                }
                else if(get_method() != "DELETE")
                {

                    full_path = Server.root + "/";
                     if(Server.index.size() > 0)
                            full_path+= Server.index[0];
                    else
                        listning_file = 1;

                }
                else
                {
                    if(!check_error(403))
                        full_path = "Errors/error403.html";
                    status_code = "403 Forbidden";
                }
            }
            }
    }
    if(check_UrlLength())
    {
        if(check_error(414))
            return 1;
        else
            this->full_path = "Errors/error414.html";
        this->status_code = "414 Request-URI Too Long";
    }
    if(Server.host.empty() || Server.root.empty() || http_version.empty() || http_version!= "HTTP/1.1")
    {
        listning_file = 0;
        if(!check_error(400))
            full_path = "Errors/error400.html";
        status_code = "400 Bad Request";
    }

    if(access(full_path.c_str(),F_OK) == 0 )
    {

            if(this->status_code.empty())
                this->status_code = "200 OK";

    }
    else
    {
      
        if(check_error(404))
            return 1;
        else
            this->full_path = "Errors/error404.html";
        this->status_code = "404 Not Found";
    }
      if(get_method() == "POST" && body_length ==0 && get_post_method() != "Chunked")
    {
        listning_file = 0;
        if(!check_error(400))
            this->full_path = "Errors/error400.html";
        this->status_code = "400 Bad Request";
    }

   full_path = get_pure_url(full_path);
    if(!check_file(full_path.c_str()))
    {
        if(!check_error(404))
            this->full_path = "Errors/error404.html";
        this->status_code = "404 Not Found";
    }
    if(url == "/signup" || url == "/log")
    {
        this->status_code = "200 OK";
    }
    
    if (full_path.find("/login/home") != std::string::npos) 
    { 
        for (std::vector<std::pair<std::string, std::string> >::const_iterator it = variables.begin(); it != variables.end(); ++it) 
        {
            if (it->first == "User-Agent" && (it->second.find("Siege")!= std::string::npos || it->second.find("Postman") != std::string::npos || it->second.find("Curl") != std::string::npos)) 
            {
                kaynaSiege = 1;
                break;
            }
        }
        if (!kaynaSiege) 
            minikamal = -1;
    }
    hhh=0;
    return 0 ;
}
std::string Request::get_server_name()
{
    return(Server.server_name[0]);
}

int Request::open_File()
{
    std::string p = get_path();
    if(!location_resp.empty() || listning_file == 1)
        return (-1);
    if(access(p.c_str(), R_OK) == 0) 
    {
      fd_file = open(p.c_str() ,O_RDWR | O_CREAT, 0666);
    }
    else if (url != "/signup" && url != "/log")
    {
        if(!check_error(403))
            full_path = "Errors/error403.html";
        status_code = "403 Forbidden";
        fd_file = open(full_path.c_str() ,O_RDWR | O_CREAT, 0666);
    }
      return (1);
}

std::string Request::type_chunked(std::string &chunk)
{

   if (chunk.size() >= 8) {
    if (chunk[0] == (char)0xFF && chunk[1] == (char)0xD8 && chunk[2] == (char)0xFF) {
        return ".jpeg";
    } else if (chunk[0] == 'P' && chunk[1] == 'K' && chunk[2] == 3 && chunk[3] == 4) {
        return ".zip";
    } else if (chunk.substr(0, 7) == "Rar!����") {
        return ".rar";
    } else if (chunk.substr(0, 4) == "%PDF") {
        return ".pdf";
    } else if (chunk.substr(0, 4) == "RIFF" && chunk.substr(8, 4) == "WAVE") {
        return ".wav";
    } else if (chunk.substr(4, 4) == "ftyp") {
        return ".mp4";
    } else if (chunk[0] == 'G' && chunk[1] == 'I' && chunk[2] == 'F' && chunk[3] == '8') {
        return ".gif";
    } else if (chunk[0] == 'B' && chunk[1] == 'M') {
        return ".bmp";
    } else if (chunk[0] == (char)0x89 && chunk[1] == 'P' && chunk[2] == 'N' && chunk[3] == 'G') {
        return ".png";
    } else if (chunk[0] == 'M' && chunk[1] == 'Z') {
        return ".exe";
    } else if (chunk[0] == 'I' && chunk[1] == 'D' && chunk[2] == '3') {
        return ".mp3";
    }
    }
    return ".Unknown";
}
std::string Request::help_function(std::string file, std::string l, std::string name)
{
    if(check_file(std::string(file + l + name).c_str()) == 1)
    {
        
        l.append("_");
        return help_function(file,l,name);
    }
    return (file + l + name);
}
int    Request::handle_post(int fd)
{
        std::string body;
        size_t j = 0;
        size_t i = 0;
        ssize_t len;
         if(get_post_method().find("Chunked")!= std::string::npos)
            body_length = 0;
        
        if(read_b < body_length || get_post_method().find("Chunked") != std::string::npos)
        {
            
            len = recv(fd,buffer,sizeof(buffer) - 1,0);
            if (len < 0)
            {
                if(time(NULL) - chrono_ >= 10)
                {
                    display_time(time(NULL));
                    std::cout << RED_COLOR << "ERROR\n\t\t\t[Empty Body]" << std::endl;
                    return(-1);
                }
            }
            if(len > 0)
            {
                read_b +=len;
                if(read_b > Server.max_body)
                {
                        if(!check_error(413))
                            this->full_path = "Errors/error413.html";
                        this->status_code = "413 Request Entity Too Large";
                        this->fd_file = open(full_path.c_str(),O_CREAT,O_RDWR,0666);
                        return 1;
                }
                buffer[len] = 0;
                body_content.append(buffer,len);
                
            }
        }
        if(get_post_method() == "Boundary" && get_url().find("cgi") == std::string::npos)
        {
            if((i = body_content.find("name")) != std::string::npos&& fileupl == -1)
            {
                            std::string tmp = body_content.substr(i + 6 , body_content.find("\"\r\n",i) - (i + 6));
                            if((j = tmp.find("filename")) != std::string::npos)
                                tmp = tmp.substr(j + 10, tmp.length() - 4);
                    filename_boundary =  help_function(upload_path , "/", tmp);

                
                body_content = body_content.substr(body_content.find("\r\n\r\n",i) + 4, body_content.length());
                
                file.open(filename_boundary.c_str(),std::ios::binary);
                 if (!file.is_open()) {
                     std::cerr << "Failed to open the file " <<  std::endl;
                     fd_file = open("Errors/error403.html",O_CREAT| O_RDWR,0666);
                     status_code = "403 Forbidden";
                     return (1);

    
                }
                if((i = body_content.find("--" + get_boundary())) != std::string::npos && (((j = body_content.find("--" + get_boundary() + "--")) == std::string::npos)|| ((j = body_content.find("--" + get_boundary() + "--")) != std::string::npos &&i != j)))
                {
                    file << body_content.substr(0,i - 2);
                    body_content = body_content.substr(i + boundary.length() + 2);
                    file.close();
                    fileupl = -1;
                   
                    return (0);
                }
                else if((i = body_content.find("--" + get_boundary() + "--")) != std::string::npos)
                {
                    file << body_content.substr(0,i - 2);
                    
                    return (1);
                }
                else
                    file<<body_content;

                fileupl = 1;
                body_content.clear();
                
                return (0);
            }
            else if(fileupl == 1)
            {
                if((i = body_content.find("--" + get_boundary() + "--")) != std::string::npos) 
                {
                
                    file << body_content.substr(0,i - 2);
                    
                    return (1);
                }
                else if((i = body_content.find("--" + get_boundary())) != std::string::npos && (((j = body_content.find("--" + get_boundary() + "--")) == std::string::npos)|| ((j = body_content.find("--" + get_boundary() + "--")) != std::string::npos &&i != j)))
                {
                    file << body_content.substr(0,i - 4);
                    body_content = body_content.substr(i + boundary.length() + 2);
                    fileupl = -1;
                    file.close();
                    return (0);
                }
                file << body_content;
            
                if(read_b >= body_length || len == 0)
                {
                    
                  
                    
                    return (1);
                }
            }
        
                body_content.clear();
                
                return (0);

        }
else if (get_post_method() == "Chunked")
        {
            std::string data;
            reserv_chunked.append(body_content);
            size_t size = 0;
            while(reserv_chunked.length() > 0)
            {
            if(count_chunked == 0)
            {
                size = reserv_chunked.find("\r\n");
                if(size != std::string::npos)
                {
                     std::sscanf(reserv_chunked.substr(0, size).c_str(), "%lx", &body_length);
                    if(body_length == 0)
                    {
                        std::string name_f = help_function(upload_path, "/chunked_data" , type_chunked(chunked_data));
                        std::ofstream file_ch;
                         file_ch.open(name_f.c_str(),std::ios::binary);
                 if (!file_ch.is_open()) {

                     fd_file = open("Errors/error403.html",O_CREAT| O_RDWR,0666);
                     status_code = "403 Forbidden";
                     return (1);
                 }
                    file_ch << chunked_data;
                    file_ch.close();
                        
                        return (1);
                    }
                    last_count_chunked = body_length;
                    reserv_chunked= reserv_chunked.substr(size + 2);
                    count_chunked = 1;
                }

            }
            if(count_chunked == 1)
            {
                if(size_t(last_count_chunked) == body_length)
                {
                     data =reserv_chunked.substr(0, body_length);
                    if(data.length() == body_length)
                    {
                        chunked_data.append(data);
                        reserv_chunked = reserv_chunked.substr(body_length + 2);
                        count_chunked =0;
                    }
                    else
                    {

                        last_count_chunked -= data.length();
                        chunked_data.append(data);
                        reserv_chunked = reserv_chunked.substr(data.length());

                    }
                }
                else if(last_count_chunked >0)
                {
                    data =reserv_chunked.substr(0,last_count_chunked);
                    chunked_data.append(data);
                    reserv_chunked = reserv_chunked.substr(data.length());
                    last_count_chunked-= data.size();
                }
                if(last_count_chunked <= 0)
                {
                    reserv_chunked = reserv_chunked.substr(2);
                    count_chunked = 0;
                }
            }
            }
            
            body_content.clear();
            
                return (0);

        }


            else
            {

                int i = check_data_type();

         
         
                switch (i)
                {
                case 1: form_data(body_content);
                    break;
                case 2: json_data();
                    break;
                case 3: (text_plain());
                    break;
                } 

            }
        if(read_b < body_length)
            return (0);
        else
        {
                  std::string name_f = help_function(upload_path,"/" ,"data");
                    std::ofstream file_ch;

                     file_ch.open(name_f.c_str());
                 if (!file_ch.is_open()) {

                     fd_file = open("Errors/error403.html",O_CREAT| O_RDWR,0666);
                     status_code = "403 Forbidden";
                     return (1);
                 }
                    for (std::map<std::string, std::string>::const_iterator it = cgi_data.begin(); it != cgi_data.end(); ++it) {

                        file_ch << "Key: " << it->first << " ";
                        file_ch << "Value: " << it->second << std::endl;
                    }
                    
                    file_ch.close();
            return (1);
        }
}
int Request::init_method()
{
    if(this->get_method() == "GET")
    {

    }
    else if(this->get_method() == "POST")
    {

    }
    else if(this->get_method() == "DELETE")
    {

    }
    return 0;
}
std::string Request::get_path()
{
    check_url();
    return (full_path);
}
size_t Request::get_body_length()

{
    return body_length;
}
Request::~Request()
{
}