#ifndef STD_LIBS_HPP
# define STD_LIBS_HPP

# include <sys/types.h>
# include <sys/socket.h> // sockaddr_storage, socklen_t
# include <netdb.h>
# include <unistd.h>
# include <signal.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <poll.h>
# include <stdlib.h>
# include <arpa/inet.h> // INET6_ADDRSTRLEN macro,

# include <string>
# include <fstream>
# include <cerrno>
# include <cstring>
# include <vector>
# include <iostream>

# include <sstream>
# include "constantes.hpp"
# include <dirent.h>
# include <sys/stat.h> // +> Si on emploie la V2 uniquement
# include <cstdio>

#endif // STD_LIBS_HPP