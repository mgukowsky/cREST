#include "OptionHandler.h"

__stdcall OptionHandler::OptionHandler() {
	setDefaults();
}

void __stdcall OptionHandler::setDefaults() {
	_settings.method = "GET";
	_settings.path = "/";
}

void __stdcall OptionHandler::setOption(int optionID, std::string value) {
	switch (optionID) {
		case OPTION_METHOD:
			_settings.method = value;
			break;
		case OPTION_PATH:
			_settings.path = value;
			break;
		default:
			break;
	}
}

const std::string __stdcall OptionHandler::getOption(int optionID) const {
	switch (optionID) {
		case OPTION_METHOD:
			return _settings.method;
		case OPTION_PATH:
			return _settings.path;
		default:
			return "";
	}
}

const std::string __stdcall OptionHandler::getOption(int optionID) {
	return static_cast<const OptionHandler*>(this)->getOption(optionID);
}