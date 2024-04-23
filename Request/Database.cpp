#include"Request.hpp"
#include"../response/response.hpp"
#include"../Server/Servers.hpp"
#include <ctime>

int Servers::check_register(std::string first, std::string last, int fd)
{
    for(unsigned long int i = 0; i < database.size(); i++)
    {
        if(database[i].firstName == first)
            if(database[i].lastName == last)
            {    
                (*request_map)[fd].datab = database[i];
                return(1);
            }
    }
    return(0);
}

int Servers::ft_register(std::map<std::string,std::string> &data, int fd)
{
    
    if(!check_register(data["firstName"],data["lastName"], fd))
    {
        Database db;
        db.firstName = data["firstName"];
        db.lastName = data["lastName"]; 
        db.username = db.firstName[0] + db.lastName;
         static const char alphanum[] =
            "KamalSafae080623";

        std::srand(static_cast<unsigned int>(std::time(NULL)));

        for (int i = 0; i < 8; ++i)
            db.password += alphanum[std::rand() % (sizeof(alphanum) - 1)];
        (*request_map)[fd].datab = db;
        this->database.push_back(db);
        return 0;
    }
    return 1;
}

int Servers::ft_login(std::map<std::string,std::string> &data,int fd)
{
    for(unsigned long int i = 0; i < database.size(); i++)
    {
        if(database[i].username == data["userName"])
        {
            if(database[i].password == data["password"])
            {
                (*request_map)[fd].datab = database[i];
                display_time(time(NULL));
                std::cout << GREEN_COLOR << "LOGIN SUCCESSFUL\n\t\t\t[" <<database[i].username << "] IS HERE." << std::endl << RESET_COLOR;
                return(0);
            }
        }
    }

    data.erase("userName");
    data.erase("password");
    (*request_map)[fd].sess_id.clear();

    return(1);
}

int Servers::main_database(std::map<std::string,std::string> &data, int fd)
{
    int res;
    if((*request_map)[fd].get_url().find("sign") != std::string::npos && (*request_map)[fd].status_code.find("414") == std::string::npos)
    {
        res=ft_register(data,fd);
        return (res);

    }
    else if((*request_map)[fd].get_url().find("log") != std::string::npos )
    {
        res = ft_login(data,fd);
        return (res);
    }
    return(-1);
}