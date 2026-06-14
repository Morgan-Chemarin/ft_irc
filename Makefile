NAME                    =   ircserv
CXX                             =   c++
CXXFLAGS                =   -Wall -Wextra -Werror -std=c++98 -I$(HEADERS_DIR) -MMD -MP

RESET           =   \033[0m
RED             =   \033[31m
GREEN           =   \033[32m
BLUE            =   \033[34m

SRCS_FILES      =   main.cpp \
					Server.cpp \
					Client.cpp \
					Parser.cpp 

SRCS_DIR        =       src
HEADERS_DIR     =       includes
OBJ_DIR			=       obj

SRCS			=       $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))
OBJS			=       $(addprefix $(OBJ_DIR)/, $(SRCS_FILES:.cpp=.o))
DEPS            =       $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
				@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
				@echo "$(GREEN)Successfully compiled [$(NAME)]$(RESET)"

-include $(DEPS)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp Makefile
		@mkdir -p $(OBJ_DIR)
		@$(CXX) $(CXXFLAGS) -c $< -o $@
		@echo "$(BLUE)Compiling $<$(RESET)"

clean:
		@rm -rf $(OBJ_DIR)
		@echo "$(RED)Object files removed.$(RESET)"

fclean: clean
		@rm -f $(NAME)
		@echo "$(RED)Executable [$(NAME)] removed.$(RESET)"

re: fclean all

.PHONY: all clean fclean re