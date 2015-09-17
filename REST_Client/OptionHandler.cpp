#include "OptionHandler.h"

__stdcall OptionHandler::OptionHandler() {
	setDefaults();
}

void __stdcall OptionHandler::setDefaults() {
	_settings.method = "GET";
}

const std::string& __stdcall OptionHandler::getOption(int optionID) const {
	switch (optionID) {
		case OPT_METHOD:
			return _settings.method;
		default:
			return "";
	}
}

const std::string& __stdcall OptionHandler::getOption(int optionID) {
	return static_cast<const OptionHandler*>(this)->getOption(optionID);
}