NAME	=	webserv
CC		=	c++ \
-Wall -Wextra -Werror -std=c++98

S		=	Srcs/
O		=	Objs/

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
			Srcs/Utils/autoIndex.cpp \
			Srcs/Utils/cgi.cpp

OBJS 	= 	$(SRCS:$S%=$O%.o)


all		:	$(NAME)

$O		:
			@mkdir $@
			@mkdir $@/Classes
			@mkdir $@/Utils

$(OBJS)	:	| $O
$(OBJS)	:	$O%.o: $S%
			$(CC) $(INCL) -c $< -o $@

$(NAME)	:	${OBJS}	
			${CC} ${INCL} $^ -o ${NAME}

clean	:
			rm -rf $O

fclean 	:	clean
			rm -rf ${NAME}

re		: fclean all

.PHONY	: all clean fclean re