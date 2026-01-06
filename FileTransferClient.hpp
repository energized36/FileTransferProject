//
// Created by Michael Lau on 2026-01-05.
//

#ifndef FILETRANSFERCLIENT_HPP
#define FILETRANSFERCLIENT_HPP

#include <netdb.h>
#include <string>


class FileTransferClient {

private:
    int sock;
    sockaddr_in server;
    int msgsock;
    char buf[1024];
    hostent *hp;
    const char *host;
    int rval;

public:
    FileTransferClient(const std::string& serverAddress);
    ~FileTransferClient();
};



#endif //FILETRANSFERCLIENT_HPP
