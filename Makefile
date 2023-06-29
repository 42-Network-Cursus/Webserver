NAME	=	webserv
INCL	=	-I ./Includes/
CC		=	c++ -Wall -Wextra -Werror -std=c++98
SRCS	=	Srcs/webserv.cpp \
			Srcs/utils.cpp \
			Srcs/configuration.cpp \
			Srcs/server.class.cpp \
			Srcs/response.class.cpp \
			Srcs/response_header.class.cpp \
			Srcs/request.class.cpp
			
OBJS 	= 	$(SRCS:.c=.o)


all		:	$(NAME)

$(NAME)	:	${OBJS}	
			${CC} ${OBJS} ${INCL} -o ${NAME}

clean :

fclean :

re :

.PHONY : all clean fclean re