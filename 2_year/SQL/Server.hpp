#ifndef SERVER 
#define SERVER

#include <sys/un.h>
#include <string>
#include <vector>

#include "Table.hpp"

using std::string;
using std::vector;

class Server{
    int sock_fd;
    struct sockaddr_un addr;
public:
    static int leng;

    Server(const char *, int, int);
    ~Server();

    void s_listen(int);
    int s_accept();
    void s_send(int, const void *, int, unsigned int);
    void s_send_vector(int, vector<string> &, unsigned int);
    void s_recv(int, void *, int, unsigned int);
    void s_shutdown(int, int);
    void s_close (int);
    
    string get_message(int);
};

#endif 
