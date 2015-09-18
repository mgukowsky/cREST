#ifndef __MAG_PROJECT_OPTIONHANDLER_H__
#define __MAG_PROJECT_OPTIONHANDLER_H__

#include <string>
#include "constants.h"

//All options are stored as strings.
class OptionHandler {
private:
	struct _options {
		std::string method;
	} _settings;
public:
	__stdcall OptionHandler();
	void __stdcall setDefaults();
	void __stdcall setOption(int optionID, std::string value);
	const std::string __stdcall getOption(int optionID) const;
	const std::string __stdcall getOption(int optionID);
};

#endif