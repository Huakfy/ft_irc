# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/18 11:30:07 by mjourno           #+#    #+#              #
#    Updated: 2023/11/28 17:17:13 by mjourno          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	ircserv
NAME_B	=	ircserv_bonus
CC		=	c++
FLAG	=	-Wall -Wextra -Werror -std=c++98 -g3

SRC_DIR	=	src/
OBJ_DIR	=	obj/

#Source
FILES	=	main Channel Client Server utils commands invite join mode nick part \
			pass privmsg quit topic user kick
SRCS	=	$(addprefix $(SRC_DIR), $(addsuffix .cpp, $(FILES)))
OBJS	=	$(addprefix $(OBJ_DIR), $(addsuffix .o, $(FILES)))
#Bonus
FILES_B	=	main Channel Client Server utils commands invite join mode_bonus nick part \
			pass privmsg quit topic user kick
SRCS_B	=	$(addprefix $(SRC_DIR), $(addsuffix .cpp, $(FILES_B)))
OBJS_B	=	$(addprefix $(OBJ_DIR), $(addsuffix .o, $(FILES_B)))

INCLUDE	=	-I ./src/class -I ./include
HNAME	=	ft_irc Channel Client Server
HEADER	=	$(addsuffix .hpp, $(HNAME))

OBJF	=	.cache_exits

$(OBJF) :
	@mkdir -p $(OBJ_DIR)

vpath %.cpp $(SRC_DIR) $(SRC_DIR)class $(SRC_DIR)commands
vpath %.hpp include/ $(SRC_DIR)class

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(FLAG) $(OBJS) $(INCLUDE) -o $(NAME)
	@echo "$(GREEN)Compiled!$(DEF_COLOR)"
	@echo "Launch the program with $(RED)./${NAME} $(DEF_COLOR)<port> <password>"

$(NAME_B) : $(OBJS_B)
	$(CC) $(FLAG) $(OBJS_B) $(INCLUDE) -o $(NAME_B)
	@echo "$(GREEN)Compiled!$(DEF_COLOR)"
	@echo "Launch the program with $(RED)./${NAME_B} $(DEF_COLOR)<port> <password>"

$(OBJ_DIR)%.o : %.cpp $(HEADER) Makefile | $(OBJF)
	$(CC) $(FLAG) $(INCLUDE) -c $< -o $@

bonus : $(NAME_B)

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
