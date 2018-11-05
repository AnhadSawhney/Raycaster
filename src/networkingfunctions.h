/*
 * networkingfunctions.h
 *
 *  Created on: Nov 3, 2018
 *      Author: Anhad
 */

#ifndef NETWORKINGFUNCTIONS_H_
#define NETWORKINGFUNCTIONS_H_

/**********************************************************************
Name		: Example UDP Client
Author		: Sloan Kelly
Date		: 2017-12-16
Purpose		: Example of a bare bones UDP client

***********************************************************************/
//#include <w32api.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

int inet_pton(int af, const char *src, void *dst)
{
  struct sockaddr_storage ss;
  int size = sizeof(ss);
  char src_copy[INET6_ADDRSTRLEN+1];

  ZeroMemory(&ss, sizeof(ss));
  strncpy (src_copy, src, INET6_ADDRSTRLEN+1);
  src_copy[INET6_ADDRSTRLEN] = 0;

  if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
    switch(af) {
      case AF_INET:
    *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
    return 1;
      case AF_INET6:
    *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
    return 1;
    }
  }
  return 0;
}

SOCKET out;
char * recvInfo() {
    sockaddr_in s;
    char * buf = (char *)malloc(1050);
    int i = sizeof(s);
    recvfrom(out, buf, 1056, 0, (sockaddr  *) &s, &i);
    return buf;
}
void shoot() {
    sockaddr_in server;
    server.sin_family = AF_INET; // AF_INET = IPv4 addresses
    server.sin_port = htons(8000); // Little to big endian conversion
    inet_pton(AF_INET, "192.168.43.231", &server.sin_addr);
    int i = sizeof(server);
    sendto(out, "shoot", 5, 0, (sockaddr *)&server, i);
}
/*void move(double x, double y, double angle) {
    sockaddr_in server;
    server.sin_family = AF_INET; // AF_INET = IPv4 addresses
    server.sin_port = htons(8000); // Little to big endian conversion
    inet_pton(AF_INET, "10.61.24.152", &server.sin_addr);
    int i = sizeof(server);
    //std::string s = (int)(x * 10) + (int)(y * 10) + (int)(angle * 10);
    const char * c = s.c_str();
    cout << c;
    sendto(out, c, 15, 0, (sockaddr *)&server, i);
}*/
void shootT(std::string ip) {
    sockaddr_in server;
    server.sin_family = AF_INET; // AF_INET = IPv4 addresses
    server.sin_port = htons(8000); // Little to big endian conversion
    inet_pton(AF_INET, "10.61.24.152", &server.sin_addr);
    int i = sizeof(server);
    std::string s2 = "shootT " + ip;
    const char * buffer = (char *)malloc(1024);
    buffer = s2.c_str();
    sendto(out, buffer, 20, 0, (sockaddr *)&server, i);
}


void setup() // We can pass in a command line option!!
{
	////////////////////////////////////////////////////////////
	// INITIALIZE WINSOCK
	////////////////////////////////////////////////////////////

	// Structure to store the WinSock version. This is filled in
	// on the call to WSAStartup()
	WSADATA data;

	// To start WinSock, the required version must be passed to
	// WSAStartup(). This server is going to use WinSock version
	// 2 so I create a word that will store 2 and 2 in hex i.e.
	// 0x0202
	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		// Not ok! Get out quickly
		cout << "Can't start Winsock! " << wsOk;
		return;
	}

	////////////////////////////////////////////////////////////
	// CONNECT TO THE SERVER
	////////////////////////////////////////////////////////////

	// Create a hint structure for the server
	sockaddr_in server;
	server.sin_family = AF_INET; // AF_INET = IPv4 addresses
	server.sin_port = htons(8000); // Little to big endian conversion
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); // Convert from string to byte array

													   // Socket creation, note that the socket type is datagram
	out = socket(AF_INET, SOCK_DGRAM, 0);

	// Write out to that socket
	int sendOk = sendto(out, "Joining", 9, 0, (sockaddr*)&server, sizeof(server));
	std::cout << WSAGetLastError();
	if (sendOk == SOCKET_ERROR) {
		cout << "FATAL ERROR " << WSAGetLastError() << endl;
	}
}


#endif /* NETWORKINGFUNCTIONS_H_ */
