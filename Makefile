NAME        = ircserv
CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): src/main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(NAME)

clean:
	rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re