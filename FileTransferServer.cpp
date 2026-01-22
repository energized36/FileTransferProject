//
// Created by Michael Lau on 2026-01-05.
//

#include "FileTransferServer.hpp"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/upnperrors.h>

FileTransferServer::FileTransferServer() {}
FileTransferServer::~FileTransferServer() {}

UPNPUrls urls;
IGDdatas data;
bool g_upnpReady = false;

bool addPortMapping(int port) {
    UPNPDev* devlist = nullptr;
    char lanaddr[64];
    char wanaddr[64];
    int error = 0;

    devlist = upnpDiscover(2000,nullptr,nullptr,0,0,2,&error);

    if (!devlist) {
        std::cerr << "No UPnP devices found\n";
        freeUPNPDevlist(devlist);
        return false;
    }

    // int r = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr), wanaddr, sizeof(wanaddr));
    int r = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));
    freeUPNPDevlist(devlist);

    // print connected IP addresses
    if (r <= 0) {
        std::cerr << "No valid IGD found\n";
        return false;
    }

    g_upnpReady = true;

    std::cout << "LAN IP: " << lanaddr << "\n";
    std::cout << "WAN IP: " << wanaddr << "\n";

    std::string portStr = std::to_string(port);
    const char* externalPort = portStr.c_str();
    const char* internalPort = portStr.c_str();
    const char* internalClient = lanaddr;
    const char* protocol = "TCP";
    const char* description = "My C++ Server";

    r = UPNP_AddPortMapping(urls.controlURL,data.first.servicetype,externalPort,internalPort,
        internalClient,
        description,
        protocol,
        nullptr,
        "0");

    if (r != UPNPCOMMAND_SUCCESS) {
        std::cerr << "AddPortMapping failed: "
                  << strupnperror(r) << "\n";
        return false;
    }
    std::cout << "Port mapping added successfully!\n";
    return true;
}

void removePortMapping(int port) {
    if (!g_upnpReady) return;

    std::string portStr = std::to_string(port);

    int r = UPNP_DeletePortMapping(
        urls.controlURL,
        data.first.servicetype,
        portStr.c_str(),
        "TCP",
        nullptr
    );

    if (r != UPNPCOMMAND_SUCCESS) {
        std::cerr << "RemovePortMapping failed: "
                  << strupnperror(r) << "\n";
    } else {
        std::cout << "Port mapping removed\n";
    }

    FreeUPNPUrls(&urls);
    g_upnpReady = false;
}


int main() {
    int sock, length;
    struct sockaddr_in server;
    int msgsock;
    char buf[1024];
    int rval;

    sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("opening stream socket");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(55387);

    if (bind (sock, (struct sockaddr *)&server, sizeof server) < 0) {
        perror ("binding stream socket");
    }

    // Use UPNP library to map ports
    addPortMapping(55387);

    listen (sock, 5);

    msgsock = accept(sock, (struct sockaddr *)0, (socklen_t *)0);
    if (msgsock == -1) {
        perror("accept");
    }

    if ((rval = read(msgsock, buf, 1024)) < 0){
        perror("reading socket");
    } else {
        printf("%s\n",buf);
    }

    strcpy(buf,"HTTP/1.1 200 OK\r\nContent-length: 20\r\nContent-type: text/plain\r\n\r\ntemp1.txt, temp2.txt");
    if ((rval = write(msgsock, buf, 1024)) < 0){
        perror("writing socket");
    }
    close(msgsock);
    removePortMapping(55387);
    return 0;
}