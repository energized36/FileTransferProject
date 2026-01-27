//
// Created by Michael Lau on 2026-01-26.
//

#include <iostream>
#include <string>
#include <fstream>
#include <ostream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

#define PORT 60001

//FileTransferServer::FileTransferServer() {}
//FileTransferServer::~FileTransferServer() {}

//UPNPUrls urls;
//IGDdatas data;
//bool g_upnpReady = false;

//bool addPortMapping(int port) {
//    UPNPDev* devlist = nullptr;
//    char lanaddr[64];
//    char wanaddr[64];
//    int error = 0;
//
//    devlist = upnpDiscover(2000,nullptr,nullptr,0,0,2,&error);
//
//    if (!devlist) {
//        std::cerr << "No UPnP devices found\n";
//        freeUPNPDevlist(devlist);
//        return false;
//    }
//
//    int r = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr), wanaddr, sizeof(wanaddr));
//    // int r = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));
//    freeUPNPDevlist(devlist);
//
//    // print connected IP addresses
//    if (r <= 0) {
//        std::cerr << "No valid IGD found\n";
//        return false;
//    }
//
//    g_upnpReady = true;
//
//    std::cout << "LAN IP: " << lanaddr << "\n";
//    std::cout << "WAN IP: " << wanaddr << "\n";
//
//    std::string portStr = std::to_string(port);
//    const char* externalPort = portStr.c_str();
//    const char* internalPort = portStr.c_str();
//    const char* internalClient = lanaddr;
//    const char* protocol = "TCP";
//    const char* description = "My C++ Server";
//
//    r = UPNP_AddPortMapping(urls.controlURL,data.first.servicetype,externalPort,internalPort,
//        internalClient,
//        description,
//        protocol,
//        nullptr,
//        "0");
//
//    if (r != UPNPCOMMAND_SUCCESS) {
//        std::cerr << "AddPortMapping failed: "
//                  << strupnperror(r) << "\n";
//        return false;
//    }
//    std::cout << "Port mapping added successfully!\n";
//    return true;
//}
//
//void removePortMapping(int port) {
//    if (!g_upnpReady) return;
//
//    std::string portStr = std::to_string(port);
//
//    int r = UPNP_DeletePortMapping(
//        urls.controlURL,
//        data.first.servicetype,
//        portStr.c_str(),
//        "TCP",
//        nullptr
//    );
//
//    if (r != UPNPCOMMAND_SUCCESS) {
//        std::cerr << "RemovePortMapping failed: "
//                  << strupnperror(r) << "\n";
//    } else {
//        std::cout << "Port mapping removed\n";
//    }
//
//    FreeUPNPUrls(&urls);
//    g_upnpReady = false;
//}


void writeTextFile(const std::string& message) {
    std::ofstream outFile("test.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing" << std::endl;
    }
    outFile << message;
    outFile.close();
}

bool send_all(int clientSocket, char* data, size_t fileSize) {
    size_t sent = 0;
    while (sent < fileSize) {
        ssize_t remainingBytes = send(clientSocket, data + sent, fileSize - sent, 0);
        if (remainingBytes <= 0) return false;
        sent += remainingBytes;
    }
    return true;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    #ifdef _WIN32
    WSADATA wsa;    
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
    #endif

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port
    #ifdef _WIN32
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    #else
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    #endif

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Listening on port " << PORT << std::endl;

    // Accept a connection
    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Read data
    while (true) {
        #ifdef _WIN32
        ssize_t n = recv(new_socket, buffer, sizeof(buffer) - 1, 0);
        #else
        ssize_t n = read(new_socket, buffer, sizeof(buffer) - 1);
        #endif
        if (n <= 0) {
            std::cout << "Client disconnected\n";
            break;
        }
        // std::string response = "GET /hello.txt HTTP/1.1\r\n"
        // "Host: localhost\r\n"
        // "\r\n";

        // Parse request, http request
        // GET,
        // DELETE,
        // POST

        // change char buffer into string for header
        std::string header(buffer);
        if (header.find("GET") != std::string::npos) {
            std::cout << "GET Request found" << std::endl;
            // GET METHOD HERE
        }
        if (header.find("POST") != std::string::npos) {
            std::cout << "POST Request found" << std::endl;
            // POST METHOD HERE
        }

        std::cout << "Message from client: " << buffer << std::endl;

        // std::string message(buffer);
        // writeTextFile(message);

        buffer[n] = '\0';
    }

    // Close socket
    #ifdef _WIN32
    closesocket(new_socket);
    closesocket(server_fd);
    #else
    close(new_socket);
    close(server_fd);
    #endif

    return 0;
}

