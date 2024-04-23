#include "Request.hpp"



int Request::extract_data_url()
{
    if(get_url().find('?') != std::string::npos)
    {
        std::vector<std::string> parser = split(get_url(),'?');
        std::vector<std::string> spliter = split(parser[1],'&');
        query_cgi = parser[1];
        extract_data(spliter);
        return(1);
    }
    return(0);
}

int Request::extract_data(std::vector<std::string> spliter )
{
    for(unsigned long i = 0; i < spliter.size() ; i++)
    {
        std::vector<std::string> parser = split(spliter[i],'=');
        querydata[parser[0]] = parser[1];
        cgi_data[parser[0]]=parser[1];

    }
    return(0);
}

int Request::form_data(std::string data)
{
    if(data.length()  <= body_length)
    {
        std::vector<std::string> spliter = split(data,'&');
        for(unsigned long i = 0; i < spliter.size() ; i++)
        {
            std::vector<std::string> parser = split(spliter[i],'=');
            querydata[parser[0]] = parser[1];
            cgi_data[parser[0]]=parser[1];
        }
        return (1);
    }
    return(0);
}

int Request::json_object()
{
size_t pos = 0;

    while (pos < get_body_content().length() && isspace(get_body_content()[pos])) 
        pos++;
    if (get_body_content()[pos] != '{')
        return -1;
    pos++;
    while (pos < get_body_content().length()) 
    {
        while (pos < get_body_content().length() && (get_body_content()[pos] == ' ' || get_body_content()[pos] == '\t' || get_body_content()[pos] == '\n' || get_body_content()[pos] == '\r')) {
            pos++;
        }
        if (get_body_content()[pos] == '"') {
            pos++;
            size_t keyStart = pos;
            while (pos < get_body_content().length() && get_body_content()[pos] != '"') 
                pos++;
            if (pos >= get_body_content().length()) 
            {
                return -1;
            }
            key = get_body_content().substr(keyStart, pos - keyStart);
            pos++;
            while (pos < get_body_content().length() && (get_body_content()[pos] == ' ' || get_body_content()[pos] == '\t' || get_body_content()[pos] == '\n' || get_body_content()[pos] == '\r'))
                pos++;
            if (get_body_content()[pos] != ':') {
                return -1;
            }
            pos++;
            if (get_body_content()[pos] == '"') {
                pos++;
                size_t valueStart = pos;
                while (pos < get_body_content().length() && get_body_content()[pos] != '"') 
                {
                    pos++;
                }
                if (pos >= get_body_content().length()) {
                    return -1; 
                }
                value = get_body_content().substr(valueStart, pos - valueStart);
                pos++;
                this->querydata[key] = value;
            } else {
                return -1;
            }
            while (pos < get_body_content().length() && (get_body_content()[pos] == ' ' || get_body_content()[pos] == '\t' || get_body_content()[pos] == '\n' || get_body_content()[pos] == '\r')) {
                pos++;
            }
            if (get_body_content()[pos] == ',') 
            {
                pos++;
            } else if (get_body_content()[pos] == '}') 
            {
                pos++;
                type_json = 1;
                return 0;
            } else {
                return -1; 
            }
        } 
        else 
            return -1;
    }

    return -1;
}

int Request::json_data()
{
    size_t pos = 0;

    while (pos < this->get_body_content().length() && isspace(this->get_body_content()[pos])) 
        pos++;
    if (pos == this->get_body_content().length()) 
        return 1;
    char firstChar = this->get_body_content()[pos];

    if (firstChar == '{') 
    {
        return(json_object());
    }
    else if (firstChar == '[') 
    {
        return(array_object());
    }
    else if (firstChar == '"') 
    {
        return(string_object());
    }
    else 
        return 1;
}
int Request::text_plain()
{
    std::istringstream data_stream(body_content);
    std::string line;
    while (std::getline(data_stream, line)) 
    {
        std::size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            cgi_data[line.substr(0, delimiter_pos)] = line.substr(delimiter_pos + 1);

        }
        else{
            cgi_data[line] = "";
        }
    }
    return 1;
}

int Request::array_object()
{
            std::string input = get_body_content(); 

        size_t pos = 0;
        size_t len = input.length();
        int key = 0; 

        while (pos < len && isspace(input[pos])) {
            pos++;
        }

        if (pos >= len || input[pos] != '[') {
            return -1;
        }

        pos++;

        while (pos < len) {
            while (pos < len && (isspace(input[pos]) || input[pos] == ' ' || input[pos] == '\t' || input[pos] == '\n' || input[pos] == '\r')) {
                pos++;
            }
            if (pos < len && input[pos] == '"') {
                pos++;
                size_t valueStart = pos;
                while (pos < len && input[pos] != '"') {
                    pos++;
                }
                if (pos >= len) {
                    return -1;
                }
                std::string value = input.substr(valueStart, pos - valueStart);
                pos++;
                std::ostringstream oss;
                oss << key++;
                std::string keyStr = oss.str();
                cgi_data[keyStr] = value;
                querydata[keyStr] = value;
            } else {
                return -1;
            }

            while (pos < len && (isspace(input[pos]) || input[pos] == ' ' || input[pos] == '\t' || input[pos] == '\n' || input[pos] == '\r')) {
                pos++;
            }

            if (pos < len && input[pos] == ',') {
                pos++;
            } else if (pos < len && input[pos] == ']') {
                pos++;
                type_json = 2;
                return 0;
            } else {
                return -1;
            }
        }

        return -1;
    }


int Request::string_object() {
        std::string input = get_body_content();
            size_t pos = 0;
        size_t len = input.length();

        while (pos < len && isspace(input[pos])) {
            pos++;
        }

        if (pos >= len || input[pos] != '{') {
            return -1; 
        }

        pos++;

        while (pos < len && isspace(input[pos])) {
            pos++;
        }

        while (pos < len && input[pos] != '}') {

            while (pos < len && (isspace(input[pos]) || input[pos] == ',')) {
                pos++;
            }

            if (pos >= len || input[pos] != '"') {
                return -1; 
            }

            pos++;
            size_t keyStart = pos;

            while (pos < len && input[pos] != '"') {
                pos++;
            }

            std::string key = input.substr(keyStart, pos - keyStart);

            pos++; 

            while (pos < len && isspace(input[pos])) {
                pos++;
            }

            if (pos >= len || input[pos] != ':') {
                return -1; 
            }

            pos++; 

            while (pos < len && isspace(input[pos])) {
                pos++;
            }

            if (pos >= len || input[pos] != '"') {
                return -1; 
            }

            pos++; 
            size_t valueStart = pos;

            while (pos < len && input[pos] != '"') {
                pos++;
            }

            std::string value = input.substr(valueStart, pos - valueStart);
            cgi_data[key] = value;
            querydata[key] = value;

            pos++;

            while (pos < len && isspace(input[pos])) {
                pos++;
            }

            if (pos < len && input[pos] == ',') {
                pos++;
            }
        }

        if (pos >= len || input[pos] != '}') {
            return -1; 
        }

        return 0;
    }



int Request::check_data_type()
{
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = variables.begin(); it != variables.end(); ++it) 
    {
        if(it->first == "Content-Type")
        {
            if (it->second == "application/x-www-form-urlencoded")
                return(1);
            else if (it->second == "application/json")
                return(2);
            else if (it->second == "text/plain")
                return(3);
        }
    }
    return(0);
}


