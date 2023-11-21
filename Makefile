# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: echapus <echapus@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/18 11:30:07 by mjourno           #+#    #+#              #
#    Updated: 2023/11/21 18:25:10 by echapus          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	ircserv
CC		=	c++
FLAG	=	-Wall -Wextra -Werror -std=c++98 -g3

#Source
FILES	=	main Channel Client Server utils commands
SRC_DIR	=	src/
OBJ_DIR	=	obj/

INCLUDE	=	-I ./src/class -I ./include
HNAME	=	ft_irc Channel Client Server
HEADER	=	$(addsuffix .hpp, $(HNAME))

SRCS	=	$(addprefix $(SRC_DIR), $(addsuffix .cpp, $(FILES)))
OBJS	=	$(addprefix $(OBJ_DIR), $(addsuffix .o, $(FILES)))

OBJF	=	.cache_exits

$(OBJF) :
	@mkdir -p $(OBJ_DIR)

vpath %.cpp $(SRC_DIR) $(SRC_DIR)class
vpath %.hpp include/ $(SRC_DIR)class

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(FLAG) $(OBJS) $(INCLUDE) -o $(NAME)
	@echo "$(GREEN)Compiled!$(DEF_COLOR)"
	@echo "Launch the program with $(RED)./${NAME} $(DEF_COLOR)<port> <password>"

$(OBJ_DIR)%.o : %.cpp $(HEADER) Makefile | $(OBJF)
	$(CC) $(FLAG) $(INCLUDE) -c $< -o $@

clean :
	@rm -rf $(OBJ_DIR)
	@rm -rf $(OBJF)

fclean : clean
	@rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re

# Color

DEF_COLOR	= \033[0;39m
RED 		= \033[0;91m
GREEN 		= \033[0;92m
