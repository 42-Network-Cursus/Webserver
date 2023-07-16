NAME	=	webserv
CC		=	c++ \
#-Wall -Wextra -Werror -std=c++98

INCL	=	-I ./Includes/ \
			-I ./Includes/Classes/

SRCS	=	Srcs/main.cpp \
			Srcs/pollin.cpp \
			Srcs/pollout.cpp \
			\
			Srcs/Classes/server.cpp \
			Srcs/Classes/response.cpp \
			Srcs/Classes/response_header.cpp \
			Srcs/Classes/request.cpp \
			Srcs/Classes/location.cpp \
			\
			Srcs/Utils/configuration.cpp \
			Srcs/Utils/utility.cpp \
			Srcs/Utils/cgi.cpp

OBJS 	= 	$(SRCS:.c=.o)


all		:	$(NAME)

$(NAME)	:	${OBJS}	
			${CC} ${OBJS} ${INCL} -o ${NAME}

clean :

fclean :

re :
	rm -rf webserv
	make all

.PHONY : all clean fclean re