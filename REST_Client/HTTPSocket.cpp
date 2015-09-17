#include "HTTPSocket.h"

WINAPI HTTPSocket::HTTPSocket(std::string url) 
	:_url(url)
{
	//std::unique_ptr<hostent> host(gethostbyname(url)); //Use unique_ptr to clean up the results of gethostbyname() automatically
	
	SecureZeroMemory(&_hints, sizeof(addrinfo));
	SecureZeroMemory(&_client, sizeof(addrinfo));
	//_hints.ai_family = AF_INET;
	//_hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(_url.c_str(), "http", &_hints, &_client);

	if (status != 0) {
		std::cerr << "Error retrieving DNS information for " << _url << std::endl << "getaddrinfo error: " << gai_strerror(status) << std::endl;
		MessageBoxA(NULL, "Error retrieving DNS information. Please check if the address is correct.", "Error!", MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL);
		_readyToFire = false;
	} else {
		_readyToFire = true;
	}
}

WINAPI HTTPSocket::~HTTPSocket() {
	int descriptor = _sock;
	closesocket(_sock);
	std::cout << "Closed socket descriptor " << descriptor << std::endl;
}

std::wstring WINAPI HTTPSocket::fireRequest(const OptionHandler& settings) {
	if (!_readyToFire) { return L"ERROR"; } //Stop if we had a DNS error

	char buf; //Use a 1 character buffer to avoid trash in the output
	addrinfo *p;
	std::stringstream ssReq;
	std::wstringstream ssRes;

	//Go through all the addresses returned by the DNS
	//http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#simpleclient
	for (p = _client; p != NULL; p = p->ai_next) {
		if ((_sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			std::cerr << "Error occured during socket()" << std::endl;
			continue;
		}

		if (connect(_sock, p->ai_addr, p->ai_addrlen) == -1) {
			closesocket(_sock);
			std::cerr << "Error occured during connect()" << std::endl;
			continue;
		}

		break;
	}

	if (p == NULL) { //We've reached the end of the linked list and still nothing.
		std::cerr << "Unable to connect to socket " << _sock << std::endl;
		return L"ERROR";
	}

	
	ssReq << settings.getOption(OPT_METHOD) << " / " << "HTTP/1.1\r\n"
		<< "Host: " << _url.c_str() << "\n"
		<< "User-Agent: magRestClient\n"
		<< "Accept: */*\n"
		<< "Connection: close\n" //Request that the server terminate the connection when it is done sending the response
		<< "\r\n\r\n";
	std::string readyToSendRequest = ssReq.str();

	if (send(_sock, readyToSendRequest.c_str(), readyToSendRequest.length(), 0) != (int)readyToSendRequest.length()) {
		std::cerr << "Error sending request to socket " << _sock << std::endl;
		return L"ERROR";
	}

	while (recv(_sock, &buf, 1, NULL) > 0) {
		ssRes << buf;
	}

	return ssRes.str();
}