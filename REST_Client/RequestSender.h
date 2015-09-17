#ifndef __MAG_PROJECT_REQUESTSENDER_H__
#define __MAG_PROJECT_REQUESTSENDER_H__

#include <iostream>
#include <thread>
#include <mutex>
#include "constants.h"
#include "HTTPSocket.h"

class RequestSender {
private:
	//The ID of the control to which the instance responds
	const int _parentID;
public:
	explicit WINAPI RequestSender(int parentID);
	WINAPI ~RequestSender();
	void WINAPI sendRequest(std::string dstURL, std::atomic<bool>& threadBool, std::wstring& response, const OptionHandler& settings);
	bool WINAPI isMatchingID(int otherID);
};

#endif