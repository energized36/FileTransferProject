//
// Created by Michael Lau on 2026-01-05.
//

#ifndef FILETRANSFERSERVER_HPP
#define FILETRANSFERSERVER_HPP
#include <netinet/in.h>


class FileTransferServer {
private:
    int sock, length;
    struct sockaddr_in server;
    int msgsock;
    char buf[1024];
    int rval;
    int i;

public:
    FileTransferServer();
    ~FileTransferServer();
};

#endif //FILETRANSFERSERVER_HPP