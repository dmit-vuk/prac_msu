#ifndef CLIENT 
#define CLIENT

#include <sys/un.h>
#include <string>
#include <vector>

class Client{
    int sock_fd;
    struct sockaddr_un addr;
public:
    static int leng;
    Client(const char *, int, int);
    
    void c_connect();
    std::string c_send(const void *, int, unsigned int);
    std::vector<std::string> c_send_recv(std::string, unsigned int);
    void stop();
    void stop_serv();
};

#endif 
