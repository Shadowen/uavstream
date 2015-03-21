#ifndef CLIENTSOCK_H
#define CLIENTSOCK_H

#include <iostream> //debugging purposes

#include <cstdlib>
#include <netdb.h>        //may or may not be necessary
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>       //may or may not be necessary

class ClientSock
{
    public:
        ClientSock(std::string server_name, int port_number);
        virtual ~ClientSock();

        //public functions
        std::string readIn(int);
        void writeOut(std::string);
        bool isConnected();
    protected:
    private:
        //private global variables
        int sockFd;
        struct sockaddr_in serAddr;
        struct hostent* server;

        bool connected;
        std::string serName;
        int portNo;

        //private functions
        bool createSock();
        void error(std::string);
};

#endif // CLIENTSOCK_H
