//
// Created by Michael Lau on 2026-01-05.
//

#include "FileTransferClient.hpp"
#include <cstdio>
#include <iostream>
#include <ostream>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>

FileTransferClient::FileTransferClient(const std::string& serverAddress) : host(serverAddress.c_str()) {
    // Create TCP socket for server, returns a file descriptor (sock)
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("opening stream socket");
    }

    // Prepare server address
    bzero(&server, sizeof(server));

    // Copy the client host name (as bytes) into the server in address socket
    hp = gethostbyname("localhost");
    bcopy((char*)hp->h_addr, (char*)&server.sin_addr, hp->h_length);

    // Convert human-readable address into binary (32 bit num)
    inet_pton(AF_INET, host, &server.sin_addr);

    // Set family to TCP protocol and set socket address of client
    server.sin_family = AF_INET;
    server.sin_port = htons(8082);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server))<0){
        perror("connecting");
    }

    // Send data to server
    // Copy text into buffer
    strcpy(buf,"GET /midp/hits HTTP/1.0\n\r\n\r");

    // Write buffer into socket and get return value
    if ((rval = write(sock, buf, 1024)) < 0){
        perror("writing socket");
    }

    // Read response from server
    if ((rval = read(sock, buf, 1024)) < 0){
        perror("reading socket");
    }
    printf("%s\n", buf);
    close (sock);
}

FileTransferClient::~FileTransferClient() {
    delete hp;
    delete host;
}

int main() {
    std::string serverAddress;
    std::cout << "enter a host address to connect: " << std::endl;
    std::cin >> serverAddress;
    FileTransferClient *c = new FileTransferClient(serverAddress);
}
