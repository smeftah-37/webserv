#include "response.hpp"
#include <signal.h>

Response::Response()
{
    
}


std::string Response::set_response_data(int fd, int i) {
   std::string response;
response += "<!DOCTYPE html>\n";
response += "<html>\n";
response += "<head>\n";
response += "    <meta charset=\"UTF-8\">\n";
response += "    <title>Webserv - FREE GAZA 🇵🇸</title>\n";
response += "    <style>\n";
response += "        body {\n";
response += "            background: linear-gradient(180deg, #ff0000, #000000, #008000);\n";
response += "            font-family: 'Lobster', cursive; /* Handwritten font */\n";
response += "            margin: 0;\n";
response += "            padding: 0;\n";
response += "            color: #ffffff;\n";
response += "            display: flex;\n";
response += "            flex-direction: column;\n";
response += "            align-items: center;\n";
response += "            justify-content: center;\n";
response += "            height: 100vh;\n";
response += "        }\n";
response += "        h1 {\n";
response += "            font-size: 36px;\n";
response += "            color: #ffffff;\n";
response += "        }\n";
response += "        h2 {\n";
response += "            font-size: 24px;\n";
response += "        }\n";
response += "        ul {\n";
response += "            list-style: none;\n";
response += "            padding: 0;\n";
response += "        }\n";
response += "        li {\n";
response += "            margin: 10px;\n";
response += "        }\n";
response += "        a {\n";
response += "            color: #ffffff;\n";
response += "            text-decoration: underline;\n";
response += "            font-size: 24px;\n";
response += "        }\n";
response += "        a:hover {\n";
response += "            text-decoration: none;\n";
response += "        }\n";
response += "    </style>\n";
response += "</head>\n";
response += "<body>\n";
response += "    <h1>Without CGI</h1>\n";
response += "    <h2>";
response += "HELLO\n";
if (i != -1) 
{
    std::map<std::string, std::string>::iterator it;
    for (it = (request_map)[fd].sess[i].data.begin(); it != (request_map)[fd].sess[i].data.end(); ++it) {
        response += it->second + " ";
    }
}
else if((request_map)[fd].check_data_type() == 1 || (request_map)[fd].check_data_type() == 3 || ((request_map)[fd].check_data_type() == 2 && (request_map)[fd].type_json == 1)  || (request_map)[fd].get_method() == "GET")
{
    std::map<std::string,std::string >::iterator it;
    for (it = (request_map)[fd].cgi_data.begin(); it != (request_map)[fd].cgi_data.end(); ++it)
    {
        response+= it->second + " ";      
    }
}
else if((request_map)[fd].check_data_type() == 2)
{
    if((request_map)[fd].type_json == 2)
    {
        for (long unsigned i = 0; i < (request_map)[fd].array_data.size(); i++ )
            response += (request_map)[fd].array_data[i] + " ";
    }
    else if((request_map)[fd].type_json == 3)
        response += (request_map)[fd].string_data + " ";
}
if((request_map)[fd].get_url().find("/signup") != std::string::npos)
{
    
response += "You are Welcome";
response += "<p><span class=\"label\">Username:</span>";
response += (request_map)[fd].datab.username;
response += "</p>";
response +=    "<p><span class=\"label\">Password:</span>";
response += (request_map)[fd].datab.password;
response +="</p>";
response +=    "<p>Please note that you cannot access your password again, so save it securely.</p>";
response +="<p>Return to <a href=\"/\">Safae Remidi's Home</a></p>";
}
else
{
    if((request_map)[fd].datab.username.empty() && i == -1)
    {
        response += "Failed : Please ensure that you have entered your username and password correctly";
        response +="<p>Return to <a href=\"/\">Safae Remidi's Home</a></p>";
    }
     else
    {
        response += "<p>Welcome ";
        if(i != -1)
            response += (request_map)[fd].sess[i].data["userName"];
        else
            response += (request_map)[fd].datab.username;
        response += "</p>";
         
        response +=    "<p> <a href=\"/login/home/index.html\">Safae Remidi's Home</a></p>";
    }
}

response += "</h2>";
response += "</body>\n";
response += "</html>\n";
return response;


}
std::string Response::set_content_directory(std::string path, int fd)
{
std::string response;
response += "<!DOCTYPE html>\n";
response += "<html>\n";
response += "<head>\n";
response += "    <meta charset=\"UTF-8\">\n";
response += "    <title>Webserv - FREE GAZA 🇵🇸</title>\n";
response += "    <style>\n";
response += "        body {\n";
response += "            background: linear-gradient(180deg, #ff0000, #000000, #008000);\n";
response += "            font-family: 'Lobster', cursive; /* Handwritten font */\n";
response += "            margin: 0;\n";
response += "            padding: 0;\n";
response += "            color: #ffffff;\n";
response += "            display: flex;\n";
response += "            flex-direction: column;\n";
response += "            align-items: center;\n";
response += "            justify-content: center;\n";
response += "            height: 100vh;\n";
response += "        }\n";
response += "        h1 {\n";
response += "            font-size: 36px;\n";
response += "            color: #ffffff;\n";
response += "        }\n";
response += "        h2 {\n";
response += "            font-size: 24px;\n";
response += "        }\n";
response += "        ul {\n";
response += "            list-style: none;\n";
response += "            padding: 0;\n";
response += "        }\n";
response += "        li {\n";
response += "            margin: 10px;\n";
response += "        }\n";
response += "        a {\n";
response += "            color: #ffffff;\n";
response += "            text-decoration: underline;\n";
response += "            font-size: 24px;\n";
response += "        }\n";
response += "        a:hover {\n";
response += "            text-decoration: none;\n";
response += "        }\n";
response += "    </style>\n";
response += "</head>\n";
response += "<body>\n";
response += "    <h1>Your files 🇵🇸</h1>\n";
response += "    <ul>\n";

DIR* dir = opendir(path.c_str());

if (dir) {
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            response += "        <li><a href=\"";
            std::string url = request_map[fd].get_url();
            if( url.length() > 0 &&  url[url.length() -1] != '/')
                url.append("/");
            response += url + entry->d_name;
            response += "\">";
            response += entry->d_name;
            response += "</a></li>\n";
        }
    }
    closedir(dir);
}

response += "    </ul>\n";
response += "</body>\n";
response += "</html>\n";

return response;

}
Response::Response(const Response& other)
{
    *this = other;
}


Response& Response::operator=(const Response& other)
{
    if (this != &other) {
        request = other.request;
        request_map = other.request_map;
        cgi = other.cgi;

    }
    return *this;
}
std::string   Response::setResponseLine(int fd)
{
    return (request_map[fd].get_http_version() + " " + request_map[fd].get_status_code() + "\r\n");
}
std::string Response::setContentType(std::string fileName, int fd)
{
    std::string ext;
    std::string contentType;
    if(fileName.empty())
        return ("text/plain");
    ext = split(fileName,'.').back();
    if(((request_map[fd].get_split_url() == "/signup" && (request_map)[fd].status_code.find("414") == std::string::npos)||request_map[fd].get_split_url() == "/log"||  ((request_map)[fd].set_session() == 0 && (request_map)[fd].cgi == 0)) && (request_map)[fd].minikamal != -1)
        contentType = "text/html";
    else if(ext == "html" || ext == "py" || request_map[fd].listning_file == 1)
    {
      contentType = "text/html";
    }
    else if(ext == "jpeg" || ext == "jpg")
        contentType = "image/jpeg";
    else if(ext == "png")
        contentType = "image/png";
    else if(ext == "gif")
        contentType = "image/gif";
    else if(ext == "mpeg")
        contentType = "audio/mpeg";
    else if(ext == "mp4")
        contentType ="video/mp4";
    else if(ext == "pdf")
    {
        contentType = "application/pdf";
    }
    else if(ext == "zip")
    {
        contentType  = "application/zip";
    }
    else
    {
        contentType = "text/plain";
    }
    return (contentType);
     
}
std::string    Response::setDate()
{
       time_t currentTime;
    time(&currentTime);
    struct tm *timeInfo = gmtime(&currentTime);
    char formattedDate[50];
    strftime(formattedDate, sizeof(formattedDate), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

    return std::string(formattedDate);

}
std::string   Response::getServerName()
{
    return (request.get_server_name());
}
Response::Response(std::map<int ,Request> rqt,int fd, int *check_c)
{
    (void)check_c;
    request_map = rqt;
    request =  (request_map)[fd];
}
int    Response::get_response(int fd,std::map<int, std::string> &fds_socket, int *check_c,std::map<int ,Request>  &req, std::vector<Session> &sessions,std::vector<Database> datab)
{
  size_t ll = 0;
    size_t i;
    signal(SIGPIPE, SIG_IGN);
    (void)fds_socket;

    if((request_map)[fd].check_resp == 0)
    {
        std::stringstream resp;
         if((request_map)[fd].set_session() == 1 || (request_map)[fd].set_session() == 0)
        {
            *check_c = 1;
        }
        if((request_map)[fd].minikamal == -1 && request_map[fd].get_method() !="DELETE")
        {
            (request_map)[fd].status_code = "403 Forbidden";
            request_map[fd].full_path = "Errors/error403.html";
        }
        resp << setResponseLine(fd);
        resp << "Date: " << setDate() << "\r\n";
        // resp << "Connection: Keep-Alive\r\n";
        resp << "Content-Type: " << setContentType((request_map)[fd].full_path,fd) << "\r\n";
        if(!request_map[fd].location_resp.empty())
        {
            resp << "Location: " << request_map[fd].location_resp << "\r\n";
        }

       
        if(*check_c == 1)
        {
            resp << "Set-Cookie: " << (request_map)[fd].cookie << "\r\n";
        }
        if(!request_map[fd].location_resp.empty() || request_map[fd].listning_file == 1 || (( ((i = (request_map)[fd].full_path.find("/login/home/files")) != std::string::npos && i == (request_map)[fd].full_path.length() - 17 )|| request_map[fd].get_split_url() == "/signup"|| request_map[fd].get_split_url() == "/log"|| (request_map)[fd].set_session() == 0)&& (request_map)[fd].minikamal != -1))
        {
            
            resp << "\r\n";
        }
        else
        {
        resp << "Transfer-Encoding: chunked" << "\r\n";
        resp << "\r\n";

        }
        if(send(fd, resp.str().c_str(), resp.str().length(), 0) <= 0)
            std::cerr<< "send eror" << std::endl;
        (request_map)[fd].check_resp = 1;
        req = request_map;
        sessions = (request_map)[fd].sess;
        if(!request_map[fd].location_resp.empty())
            return (1);
        return (0);

    }
    if( request_map[fd].listning_file == 1 ||(( i = (request_map)[fd].full_path.find("/login/home/files")) != std::string::npos && i == (request_map)[fd].full_path.length() - 17 && (request_map)[fd].minikamal != -1))
    {
        std::stringstream htmlContent; 
        htmlContent << set_content_directory(request_map[fd].full_path,fd);
        if(send(fd, htmlContent.str().c_str(), htmlContent.str().length(), 0)<= 0)
             std::cerr << "Send error" << std::endl;

        sessions = (request_map)[fd].sess;
        return (1);

    }
    else if(((request_map[fd].get_split_url() == "/signup" && (request_map)[fd].status_code.find("414") == std::string::npos)||request_map[fd].get_split_url() == "/log"||  ((request_map)[fd].set_session() == 0 && (request_map)[fd].cgi == 0)) && (request_map)[fd].minikamal != -1)
    {
    
        std::stringstream htmlContent; 
         for(unsigned long int i= 0 ; i < (request_map)[fd].sess.size(); i++)
        {
            *check_c = 0;
            if((request_map)[fd].sess_id == (request_map)[fd].sess[i].session)
            {

                htmlContent << set_response_data(fd,i);
                req = request_map;
                sessions = (request_map)[fd].sess;

                 if (send(fd, htmlContent.str().c_str(), htmlContent.str().length(), 0) <= 0)
                    std::cerr << "Send error" << std::endl;

                 return (1);
                
            }

        }
        *check_c = 1;
        htmlContent << set_response_data(fd,-1);
        if (send(fd, htmlContent.str().c_str(), htmlContent.str().length(), 0) <= 0)
            std::cerr << "Send error" << std::endl;
        req = request_map;
        sessions = (request_map)[fd].sess;
        return (1);
    }
    else
    {
        
        
        if((request_map)[fd].set_session() == 0)
        {
            for(unsigned long int i= 0 ; i < (request_map)[fd].sess.size(); i++)
            {


                if((request_map)[fd].sess_id == (request_map)[fd].sess[i].session)
                {

                    (request_map)[fd].cooki = 1;
                if(request_map[fd].t_out == 0)
                {
                    int value = -2;
                    (request_map)[fd].fd_file =  cgi.start_CGI(datab,value,(request_map)[fd].t_out,(request_map)[fd].removing_file,(request_map)[fd].id_child,request_map[fd]);
                    if((request_map)[fd].t_out ==0 )
                        return (0); 
                    else
                        break;
                }
                   cgi = CGI(&(request_map)[fd]);


                    (request_map)[fd].fd_file =  cgi.start_CGI(datab,0,(request_map)[fd].t_out,(request_map)[fd].removing_file,(request_map)[fd].id_child,request_map[fd]);
                     if((request_map)[fd].t_out == 0)
                        return (0);

                    
                }


            }

        }

    size_t CHUNK_SIZE = 4096;
     char buf[CHUNK_SIZE];
     if((request_map)[fd].minikamal == -1 && request_map[fd].get_method() !="DELETE")
        (request_map)[fd].fd_file = open("Errors/error403.html",O_RDWR | O_CREAT, 0666);
    ssize_t bytes_read = read((request_map)[fd].fd_file, buf,CHUNK_SIZE);
        if (bytes_read > 0) {
            std::ostringstream chunk_header;
            chunk_header << std::hex << bytes_read << "\r\n";
            if (send(fd, chunk_header.str().c_str(), chunk_header.str().length(), 0) == -1) {
                close(fd);
               return 1;
            }
            int i = send(fd, buf, bytes_read, 0);
            if ( i== -1) {
                close(fd);
                return 1;

            }
            ll+= i;
             write(fd, "\r\n", 2);
             if(bytes_read == 0 ||(size_t) bytes_read < CHUNK_SIZE)
             {
                write(fd, "0\r\n\r\n", 5);
                close((request_map)[fd].fd_file);
                 sessions = (request_map)[fd].sess;

                return (1);
             }
             if(bytes_read > 0)
                return 0;
        }
    }

    req = request_map;
    sessions = (request_map)[fd].sess;

     return (1);
    
}
Response::~Response()
{


}