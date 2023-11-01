# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/18 11:30:07 by mjourno           #+#    #+#              #
#    Updated: 2023/10/18 11:38:09 by mjourno          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	ircserv

CC		=	c++
FLAG	=	-Wall -Wextra -Werror -std=c++98 -g3
#Source
SRC_DIR	=
OBJ_DIR	=	obj/

INCLUDE	=	-I ./class

HDIR	=	class/
HNAME	=	Channel.hpp Client.hpp Server.hpp

# Source
FILES	=	mainre Channel Client Server

SRCS	=	$(addprefix $(SRC_DIR), $(addsuffix .cpp, $(FILES)))
OBJS	=	$(addprefix $(OBJ_DIR), $(addsuffix .o, $(FILES)))
HEADER	=	$(addprefix $(HDIR), $(HNAME))

OBJF	=	.cache_exits

$(OBJF) :
	@mkdir -p $(OBJ_DIR)

vpath %.cpp $(SRC_DIR) $(SRC_DIR)class

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(FLAG) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Compiled!$(DEF_COLOR)"
	@echo "Launch the program with $(RED)./${NAME} $(DEF_COLOR)<port> <password>"

$(OBJ_DIR)%.o : %.cpp $(HEADER) Makefile | $(OBJF)
	$(CC) $(FLAG) -c $< -o $@

clean :
	@rm -rf $(OBJ_DIR)
	@rm -rf $(OBJF)

fclean : clean
	@rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re

# Color

DEF_COLOR	= \033[0;39m
GRAY 		= \033[0;90m
RED 		= \033[0;91m
GREEN 		= \033[0;92m
YELLOW 		= \033[0;93m
BLUE		= \033[0;94m
MAGENTA		= \033[0;95m
CYAN		= \033[0;96m
WHITE		= \033[0;97m