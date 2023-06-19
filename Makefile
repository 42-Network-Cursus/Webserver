NAME	=	webserv
INCL	=	-I ./Includes/
CC		=	c++ -Wall -Wextra -Werror -std=c++98
SRCS	=	srcs/webserv.cpp \
			srcs/utils.cpp \
			srcs/server.class.cpp \
			srcs/configuration.class.cpp
OBJS 	= 	$(SRCS:.c=.o)


all		:	$(NAME)

$(NAME)	:	${OBJS}	
			${CC} ${OBJS} ${INCL} -o ${NAME}

clean :

fclean :

re :

.PHONY : all clean fclean re