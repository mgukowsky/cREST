#include "RequestSender.h"

WINAPI RequestSender::RequestSender(int parentID)
	:_parentID(parentID), _port("http") {};

WINAPI RequestSender::~RequestSender() {
	std::cout << "Request Sender with parentID " << _parentID << " was deleted.\n";
}

void WINAPI RequestSender::sendRequest(std::string dstURL, std::atomic<bool>& threadBool, std::wstring& response, OptionHandler& settings) {
	settings.setOption(OPTION_PATH, getPathAndFormat(dstURL));
	std::string portToSend = _port; //lambdas don't seem to like getting passed private instance variables...

	const DWORD parentThread = GetCurrentThreadId();

	//Be sure to pass parentThread by VALUE, b/c it will almost certainly fall out of scope and be killed in the main thread before this thread finishes.
	std::thread reqThread([dstURL, parentThread, portToSend, &threadBool, &response, &settings](){
		std::mutex mtx;
		HTTPSocket tmp(dstURL, portToSend);

		mtx.lock();
		/*----------------*/
		response = tmp.fireRequest(settings, parentThread);
		std::cout << "request sent\n";
		threadBool = true;
		/*----------------*/
		mtx.unlock();

		DWORD msgSuccess = PostThreadMessage(parentThread, SET_RESPONSE_TEXT, NULL, (LPARAM)THREAD_STATUS_DONE);
		if (msgSuccess == NULL){ //PostThreadMessage returns 0 if the message cannot be sent
			mtx.lock();
			/*----------------*/
			std::cerr << "Error communicating with parent thread" << std::endl;
			/*----------------*/
			mtx.unlock();
		}
		
		return 0;
	});

	reqThread.detach();
}

bool WINAPI RequestSender::isMatchingID(int otherID) {
	return _parentID == otherID;
}

std::string WINAPI RequestSender::getPathAndFormat(std::string& url) {
	if (url.substr(0, 7) == "http://") {
		url = url.substr(7);
		_port = "http";
	} else if (url.substr(0, 8) == "https://") {
		url = url.substr(8);
		_port = "https";
	} 

	std::size_t pathBegin = url.find('/');
	std::string outputPath;
	
	if (pathBegin == std::string::npos) {
		return "/";
	} else {
		outputPath = url.substr(pathBegin);
		url = url.substr(0, pathBegin);
		//Some servers will fail with out a trailing '/'
		return outputPath + '/';
	}

}