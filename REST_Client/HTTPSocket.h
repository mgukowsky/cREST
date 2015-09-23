#ifndef __MAG_PROJECT_HTTPSOCKET_H__
#define __MAG_PROJECT_HTTPSOCKET_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <atomic>
#include "OptionHandler.h"

//Major credit to http://stackoverflow.com/questions/17685466/http-request-by-sockets-in-c

//Abstracts away the low level Winsock procedures
class HTTPSocket {
private:
	int _sock;
	addrinfo *_client; //will be a linked list after call to getaddrinfo
	addrinfo _hints;
	bool _readyToFire;
	std::string _url, _port;
public:
	explicit WINAPI HTTPSocket(std::string url, std::string port = "http");
	WINAPI ~HTTPSocket();
	static const int PORT = 80;
	std::wstring WINAPI fireRequest(const OptionHandler& settings, const DWORD parentThread);
};

#endif