#include "Server.hpp"
#include "Table.hpp"
#include "Poliz.hpp"
#include "Parser.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::vector;

int Server::leng = 1024;

Server::Server(const char *name, int type, int protocol)
{
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, name);
    
     if ((sock_fd = socket(addr.sun_family, type, protocol)) < 0)
        throw "Server: can't create socket";
    
    unlink(name);
    
    if (bind(sock_fd, (struct sockaddr *) &addr, sizeof addr) < 0)
        throw "Can't bind socket";
}

Server::~Server()
{
    unlink(addr.sun_path);
    shutdown(sock_fd, 2);
    close(sock_fd);
}

void Server::s_listen(int num)
{
    if (listen(sock_fd, num) < 0)
        throw "Can't listen socket";
}

int Server::s_accept()
{
    int cl_sock;
    if ((cl_sock = accept(sock_fd, NULL, NULL)) < 0)
        throw "Can't accept socket";
    return cl_sock;
}

void Server::s_send(int cl_sock, const void *msg, int len, unsigned int flags)
{
    if (send(cl_sock, msg, len, flags) < 0)
        throw "Server: can't write socket";
}

void Server::s_send_vector(int cl_sock, vector<string> & res, unsigned int flags)
{
    std::stringstream ss;
    ss << res.size();
    string size = ss.str();
    s_send(cl_sock, size.c_str(), size.size(), 0);
    get_message(cl_sock);
    for (size_t i = 0; i < res.size() - 1; ++i) {
        s_send(cl_sock, res[i].c_str(), res[i].size(), 0);
        get_message(cl_sock);
    }
    s_send(cl_sock, res[res.size()-1].c_str(), res[res.size()-1].size(), 0);
}

void Server::s_recv(int cl_sock, void *msg, int len, unsigned int flags)
{
    if (recv(cl_sock, msg, len, flags) < 0)
        throw "Server: can't reed socket";
}

void Server::s_shutdown(int sock, int mode)
{
    if (shutdown(sock, mode) < 0)
        throw "Сan't shutdown socket";
}

void Server::s_close(int sock)
{
    if (close(sock) < 0)
        throw "San't close socket";
}

string Server::get_message(int cl_sock)
{
    string command = "";
    bool cmd = true;
    while (cmd) {
        char mes[leng] = {0};
        string ret = "";
        s_recv(cl_sock, &mes, sizeof(mes), 0);
        for (int i = 0; i < leng && mes[i] != '\0'; ++i)
            ret += mes[i];
        command += ret;
        if (command[command.size() - 1] == '\a') {
            cmd = false;
            command.erase(command.size() - 1);
        }
    }
    return command;
}

int main()
{
    Server ser("file.txt", SOCK_STREAM, 0);
    ser.s_listen(1);
    bool work = true;
    while (work) {
        int cl_sock;
        cl_sock = ser.s_accept();
        string command = "";
        while (1) {
            command = ser.get_message(cl_sock);
            vector<string> res;
            if (command != "stop" && command != "stop_serv") {
                try {
                    Parser par(command);
                    Poliz pol;
                    par.analyze(pol);
                    res = pol.calculate();
                } catch (const char *ex) {
                    command.clear();
                    for (int i = 0; ex[i] != '\0'; ++i)
                        command += ex[i];
                    res.push_back(command);
                } catch (string ex) {
                    res.push_back(ex);
                } catch (...) {
                    command = "Syntax error\n";
                    res.push_back(command);
                }
            }
            else {
                ser.s_send(cl_sock, "Ok", 2, 0);
                if (command == "stop_serv")
                    work = false;
                break;
            }
            ser.s_send_vector(cl_sock, res, 0);
            command.clear();
        }
        ser.s_shutdown(cl_sock, 2);
        ser.s_close(cl_sock);
        cout << "Client is disconnected\n";
    }
    return 0;
}

