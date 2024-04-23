NAME = webserv
SRC = Configfile/config_file.cpp Configfile/server.cpp Configfile/location.cpp main.cpp Request/Request.cpp Server/Servers.cpp Server/ServerTCP.cpp response/response.cpp Request/RequestData.cpp CGI/CGI.cpp Request/Cookies.cpp Request/Database.cpp

OBJ = ${SRC:.cpp=.o}
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
LDFLAGS =  
LDLIBS = 

CXX = c++

all: $(NAME)

$(NAME) : $(OBJ)
	$(CXX)  $(OBJ)   $(CXXFLAGS) $(LDFLAGS)  $(LDLIBS) -o $(NAME)
clean:
	rm -f $(OBJ)
fclean:
	rm -f $(OBJ) $(NAME)
re: fclean all