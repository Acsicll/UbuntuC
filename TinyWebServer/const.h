#include <iostream>
#include <tuple>
#include <vector>
#include <cstring>
#include <string>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using std::string;
using std::endl;
using std::cout;
using std::cin;
using std::cerr;
using std::size_t;
using std::tuple;
using std::get;

const int BUFFER_LENGTH = 1024;

int epfd;
