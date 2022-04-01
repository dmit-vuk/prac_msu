#include "Client.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

int Client::leng = 1024;

Client::Client(const char *name, int type, int protocol)
{
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, name);
    
    if ((sock_fd = socket(addr.sun_family, type, protocol)) < 0)
        throw "Client: Can't create socket";
}

void Client::c_connect()
{
    if (connect(sock_fd, (struct sockaddr *) &addr, sizeof addr) < 0)
        throw "Client: Can't connect socket";
}

string Client::c_send(const void *msg, int len, unsigned int flags)
{
    if (send(sock_fd, msg, len, flags) < 0)
        throw "Client: can't write socket";
    if (send(sock_fd, "\a", 1, flags) < 0)
        throw "Client: can't write socket";
    
    char rcv_mes[leng] = {0};
    if (recv(sock_fd, &rcv_mes, sizeof(rcv_mes), 0) < 0)
        throw "Client: can't reed socket";
    string ret = "";
    for (int i = 0; i < leng && rcv_mes[i] != '\0'; ++i)
        ret += rcv_mes[i];
    return ret;
}

vector<string> Client::c_send_recv(string str, unsigned int flags)
{
    string answ = c_send(str.c_str(), str.size(), 0);
    vector<string> res;
    size_t size = stol(answ);
    for (size_t i = 0; i < size; ++i) {
        res.push_back(c_send("Ok", 2, 0));
    }
    return res;
}

void Client::stop()
{
    const char *msg = "stop\a";
    if (send(sock_fd, msg, strlen(msg), 0) < 0)
        throw "Client: can't reed socket";
    char rcv_mes[leng] = {0};
    if (recv(sock_fd, &rcv_mes, sizeof(rcv_mes), 0) < 0)
        throw "Client: can't reed socket";
    string str(rcv_mes);
}

void Client::stop_serv()
{
    const char *msg = "stop_serv\a";
    if (send(sock_fd, msg, strlen(msg), 0) < 0)
        throw "Client: can't reed socket";
    char rcv_mes[leng] = {0};
    if (recv(sock_fd, &rcv_mes, sizeof(rcv_mes), 0) < 0)
        throw "Client: can't reed socket";
    string str(rcv_mes);
}

int main()
{
    Client cl("file.txt", SOCK_STREAM, 0);
    cl.c_connect();
    
    string str;
    vector<string> answ;
    bool cmd = true;
    
    while (cmd) {
        cout << "Enter the command\n";
        str.clear();
        int cur_char;
        bool apostr = false, stop = false;
        while (1) {
            cur_char = getchar();
            if (str.empty())
                while (isspace(cur_char))
                    cur_char = getchar();
            if (cur_char == '\'') {
                if (apostr)
                    apostr = false;
                else
                    apostr = true;
            }
            if (cur_char == ';' && !apostr)
                break;
            if (cur_char == EOF) {
                if (!str.empty())
                    str += ';';
                stop = true;
                break;
            }
            str += cur_char;
        }
        if (!str.empty() && str != "stop") {
            cout << "---------\n";
            answ = cl.c_send_recv(str, 0);
            for (size_t i = 0; i < answ.size(); ++i)
                cout << answ[i] << endl;
            cout << "---------\n";
        }
        if (stop) {
            cl.stop();
            cmd = false;
            break;
        } else if (str == "stop") {
            cl.stop_serv();
            cmd = false;
            break;
        }
    }
    
}

