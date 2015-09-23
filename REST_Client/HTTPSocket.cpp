#include "HTTPSocket.h"

WINAPI HTTPSocket::HTTPSocket(std::string url, std::string port)
	:_url(url), _port(port)
{	
	SecureZeroMemory(&_hints, sizeof(addrinfo));
	SecureZeroMemory(&_client, sizeof(addrinfo));

	int status = getaddrinfo(_url.c_str(), _port.c_str(), &_hints, &_client);

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

std::wstring WINAPI HTTPSocket::fireRequest(const OptionHandler& settings, const DWORD parentThread) {
	if (!_readyToFire) { return L"ERROR: DNS lookup failed."; } //Stop if we had a DNS error

	char buf; //Use a 1 character buffer to avoid trash in the output
	addrinfo *p;
	std::stringstream ssReq;
	std::wstringstream ssRes, ssOutput;

	PostThreadMessage(parentThread, SET_RESPONSE_TEXT, NULL, (LPARAM)THREAD_STATUS_CONNECTING);

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
		return L"ERROR: Could not establish connection to host.";
	}

	PostThreadMessage(parentThread, SET_RESPONSE_TEXT, NULL, (LPARAM)THREAD_STATUS_WAITING);

	//Build request string conforming to HTTP standards -> http://tools.ietf.org/html/rfc2616#section-14
	ssReq << settings.getOption(OPTION_METHOD) << " " << settings.getOption(OPTION_PATH) 
		<< " HTTP/1.1\r\n"
		<< "Host: " << _url.c_str() << "\r\n"
		<< "User-Agent: cREST\r\n"
		<< "Accept: */*\r\n"
		<< "Connection: close\r\n" //Request that the server terminate the connection when it is done sending the response
		<< "\r\n\r\n";
	std::string readyToSendRequest = ssReq.str();

	if (send(_sock, readyToSendRequest.c_str(), readyToSendRequest.length(), 0) != (int)readyToSendRequest.length()) {
		std::cerr << "Error sending request to socket " << _sock << std::endl;
		return L"ERROR";
	}

	bool shouldPostInitialReceive = true;

	while (recv(_sock, &buf, 1, NULL) > 0) {
		if (shouldPostInitialReceive) {
			PostThreadMessage(parentThread, SET_RESPONSE_TEXT, NULL, (LPARAM)THREAD_STATUS_RECEIVING_RESPONSE);
			shouldPostInitialReceive = false;
		}
		ssRes << buf;
	}

	//Build up formatted output to send to screen
	ssOutput << "***REQUEST***\r\n\r\n" << ssReq.str().c_str() << "\n***RESPONSE***\r\n\r\n" 
		<< ssRes.str().c_str() << "\r\n\r\n***END OF RESPONSE***";

	return ssOutput.str();
}