#include"Request.hpp"

int Request::check_sessionid(std::string session_id) {
    for (size_t i = 0; i < sess.size(); i++) 
    {
        if (sess[i].session == session_id) 
        {

        }
        if (get_url().find(sess[i].path) == std::string::npos) 
        {
            sess.erase(sess.begin() + i);
            return 0;
        }
            return 1;
        }
    return 0;
}


bool parseDateTime(const char* str, struct tm& timeinfo) {
    const char* format = "%a, %d-%b-%Y %H:%M:%S GMT";
    return (sscanf(str, format, &timeinfo.tm_wday, &timeinfo.tm_mday, &timeinfo.tm_mon, &timeinfo.tm_year, &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec) == 7);
}

bool Request::is_expired(int i,const char* str) {
    struct tm time1;
    struct tm time2;
    std::memset(&time1, 0, sizeof(time1));
    std::memset(&time2, 0, sizeof(time2));

    if (parseDateTime(str, time1) && parseDateTime(sess[i].expiration.c_str(), time2)) {
        time_t time1_t = mktime(&time1);
        time_t time2_t = mktime(&time2);

        return time1_t > time2_t;
    }

    return false;
}

int Request::parse_cookie(std::string str)
{
    std::vector<std::string> parser = split_word(str,"; ");
    for(long unsigned int i = 0; i < parser.size() ; i++)
    {
        if(parser[i].substr(0,11)== "session_id=")
        {
            sess_id = parser[i].substr(11);
             if(check_sessionid(sess_id))
                return(1);
        }
    }

    return(0);

}
int Request::check_cookies(int i)
{
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = variables.begin(); it != variables.end(); ++it) 
    {
        if(it->first == "Cookie")
        {
            minikamal = 1;
            if(i == 1)
                return 1;
            if(parse_cookie(it->second))
            {
                return(1);
            }
        }
    }

    return(0);
}

int Request::set_session()
{
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = variables.begin(); it != variables.end(); ++it) 
    {
        if(it->first == "Host")
        {
            std::vector<std::string> parser = split(it->second,':');
            this->host = parser[0];
        }
    }
    check_cookies(1);
    if(get_url() != "/login" &&  get_url().find("/login/login")== std::string::npos &&get_url() !="/log"&& get_url().find("cgi-bin/login") == std::string::npos)
    {
        
        check_c = -1;
        return (-1);
    }

    else if(!check_cookies(0) &&  check_c == -1 && !datab.username.empty() && (get_url().find("/cgi-bin/login") != std::string::npos || get_url()== "/log"))
    {
        minikamal = 1;
        static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        std::srand(static_cast<unsigned int>(std::time(NULL)));

        for (int i = 0; i <  10; ++i)
            session_id += alphanum[std::rand() % (sizeof(alphanum) - 1)];
        std::time_t now = std::time(NULL);
        std::time_t expireTime = now + 60;
        char expiresStr[80];
        std::strftime(expiresStr, sizeof(expiresStr), "%a, %d-%b-%Y %H:%M:%S GMT", std::gmtime(&expireTime));
        session_timer = expiresStr;
        session_path = "/login";
        session_.path = session_path;
        session_.session = session_id;
        session_.data["firstName"] = datab.firstName;
        session_.data["lastName"] = datab.lastName;
        session_.data["userName"] = datab.username;
        session_.data["password"] = datab.password;
        session_.expiration = session_timer;
        session_.domain_name = this->host;
        cookie = "session_id=" + session_id + "; Path=" + session_path + "; Expires=" + session_timer + "; Domain=" + this->host;
        check_c = 1;
        return(1);
    }
    else if(!sess_id.empty())
    {
        check_c = 0;
        minikamal = 0;

         for(long unsigned int i= 0 ; i < sess.size(); i++)
        {

            if(sess_id == sess[i].session)
            {
                cookie = "session_id=" +  sess[i].session + "; Path=" +  sess[i].path + "; Expires=" +  sess[i].expiration + "; Domain=" + this->host;
            }
        }
        return(0);
    }

    return -1;
}
