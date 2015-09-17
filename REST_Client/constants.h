#ifndef __MAG_PROJECT_CONSTANTS_H__
#define __MAG_PROJECT_CONSTANTS_H__

#define MASTER_WIDTH			1280
#define MASTER_HEIGHT			720

#define IDC_SEND_BUTTON		101
#define IDC_URL_ENTRY			102
#define IDC_RADIO_GET			201
#define IDC_RESPONSE_AREA 301

#define SET_RESPONSE_TEXT	0xABCD

enum ControlIDs {
	BTN_SEND, 
	URL_ENTRY,
	OPT_GET,
	AREA_RESPONSE,
	CONTROL_ID_TOTAL
};

enum OptionIDs {
	OPT_METHOD,
	OPT_TOTAL
};

#endif