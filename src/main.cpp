#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <iomanip>
#include <map>
#include <cctype>
#include <sstream>
#pragma comment(lib, "ws2_32.lib")

#include "crypto.h"

#define LISTEN_PORT 7881
#define FORWARD_PORT 7880
#define BUFFER_SIZE 4096
#define IPADDRESS "127.0.0.1"

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in listenAddr = {};
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(LISTEN_PORT);
    listenAddr.sin_addr.s_addr = inet_addr(IPADDRESS);

    bind(listenSocket, (sockaddr*)&listenAddr, sizeof(listenAddr));
    listen(listenSocket, SOMAXCONN);
    std::cout << "[*] Sniffer listening on port " << LISTEN_PORT << ". Forwarding to port " << FORWARD_PORT << "...\n";

    SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
    std::cout << "[*] Client connected.\n";

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr = { AF_INET, htons(FORWARD_PORT) };
    inet_pton(AF_INET, IPADDRESS, &serverAddr.sin_addr);
    connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    
	fd_set readfds;
	unsigned char buffer[BUFFER_SIZE];
	unsigned char decrypted[BUFFER_SIZE];

    while (true) {
		FD_ZERO(&readfds);
		FD_SET(clientSocket, &readfds);
        FD_SET(serverSocket, &readfds);

        select(0, &readfds, nullptr, nullptr, nullptr);

        if (FD_ISSET(clientSocket, &readfds)) {
			int bytesRecv = recv(clientSocket, (char*)buffer, BUFFER_SIZE, 0);
            if (bytesRecv <= 0) break;
            std::cout << "[Client->Server] Length: " << bytesRecv << "\n";

			// Decrypt the data
			DecryptData(buffer, decrypted, bytesRecv);
            
			// Decode the packet
			DecodePacket(decrypted, bytesRecv);

			// Send the decrypted data to the server
            send(serverSocket, (char*)decrypted, bytesRecv, 0);
		}

        if (FD_ISSET(serverSocket, &readfds)) {
            int bytesRecv = recv(serverSocket, (char*)buffer, BUFFER_SIZE, 0);
            if (bytesRecv <= 0) break;
            std::cout << "[Server->Client] Length: " << bytesRecv << "\n";
            
			// Decrypt the data
			DecryptData(buffer, decrypted, bytesRecv);

			// Decode the packet
			DecodePacket(decrypted, bytesRecv);

			// Send the decrypted data to the client
			send(clientSocket, (char*)decrypted, bytesRecv, 0);
        }
    
    }


    closesocket(clientSocket);
    closesocket(serverSocket);
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}