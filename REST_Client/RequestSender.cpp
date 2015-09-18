#include "RequestSender.h"

WINAPI RequestSender::RequestSender(int parentID)
	:_parentID(parentID) {};

WINAPI RequestSender::~RequestSender() {
	std::cout << "Request Sender with parentID " << _parentID << " was deleted.\n";
}

void WINAPI RequestSender::sendRequest(std::string dstURL, std::atomic<bool>& threadBool, std::wstring& response, const OptionHandler& settings) {
	const DWORD parentThread = GetCurrentThreadId();

	//Be sure to pass parentThread by VALUE, b/c it will almost certainly fall out of scope and be killed in the main thread before this thread finishes.
	std::thread reqThread([dstURL, parentThread, &threadBool, &response, &settings](){
		std::mutex mtx;
		HTTPSocket tmp(dstURL);

		mtx.lock();
		/*----------------*/
		response = tmp.fireRequest(settings, parentThread);
		std::cout << "request sent\n";
		threadBool = true;
		/*----------------*/
		mtx.unlock();

		DWORD msgSuccess = PostThreadMessage(parentThread, SET_RESPONSE_TEXT, NULL, NULL);
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