#include "main.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{

	int iResult;

	// Initialize Winsock first
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		MessageBoxA(NULL, "Unable to initialize Winsock!", "Error!", MB_ICONERROR | MB_OK);
		return 1;
	}

	HWND hwnd;
	MSG Msg;

	hDefaultFont = GetStockObject(DEFAULT_GUI_FONT);
	gHBRBackground = CreateSolidBrush(RGB(0, 0xCC, 0));

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;

	if (!RegisterClassEx(&wc))
	{
		MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(0, wc.lpszClassName, L"cREST", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
		MASTER_WIDTH, MASTER_HEIGHT, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBoxA(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		//Checks for thread communications
		//TODO: move these out of the main msg loop
		if (Msg.message == SET_RESPONSE_TEXT) {
			if (Msg.lParam == THREAD_STATUS_CONNECTING) {
				SendMessage(vControls[AREA_RESPONSE], WM_SETTEXT, NULL, (LPARAM)L"Connecting to server...");
			} else if (Msg.lParam == THREAD_STATUS_WAITING) {
				SendMessage(vControls[AREA_RESPONSE], WM_SETTEXT, NULL, (LPARAM)L"Waiting for server response...");
			} else if (Msg.lParam == THREAD_STATUS_RECEIVING_RESPONSE) {
				SendMessage(vControls[AREA_RESPONSE], WM_SETTEXT, NULL, (LPARAM)L"Receiving server response...");
			} else if (Msg.lParam == THREAD_STATUS_DONE && gThreadBool) {
				SendMessage(vControls[AREA_RESPONSE], WM_SETTEXT, NULL, (LPARAM)gThreadResponse.c_str());
				gThreadBool = false;
				gCanSend = true;
			};
		}
		//Send the request whenever ENTER is pressed, regardless of keyboard focus
		if (Msg.message == WM_KEYUP && Msg.wParam == VK_RETURN) {
			SendMessage(hwnd, WM_COMMAND, (WPARAM)IDC_SEND_BUTTON, NULL); //Emulate clicking the SEND button
		}
		//Shut the beeper up when pressing ENTER
		if (Msg.message == WM_KEYDOWN && Msg.wParam == VK_RETURN) {
			continue;
		}
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
			gRequestSender = new RequestSender(IDC_SEND_BUTTON);
			if (!setup_window(hwnd, hDefaultFont)) {
				MessageBoxA(NULL, "Failed to create dialog item(s)", "Error!", MB_ICONEXCLAMATION | MB_OK);
				return NULL;
			};
			CheckRadioButton(hwnd, IDC_RADIO_GET, IDC_RADIO_OPTIONS, IDC_RADIO_GET);
			break;
		case WM_CTLCOLORLISTBOX:
			return (LONG)gHBRBackground;
		case WM_PAINT:
			paintText(hwnd);
			break;
		case WM_COMMAND:
			handleCommand(hwnd, msg, wParam, lParam);
			break;
		case WM_CLOSE:
			delete gRequestSender;
			WSACleanup();
			PostQuitMessage(0);
			break;
		case WM_DESTROY:
			DestroyWindow(hwnd);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

bool WINAPI setup_window(HWND hwnd, HGDIOBJ hFont) {
	bool success = true;

	//One downside to using a vector is that the controls need to be pushed in the correct order
	vControls.push_back(CreateWindowExA(NULL, "BUTTON", "SEND", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		MASTER_WIDTH - 150, MASTER_HEIGHT - 420, 100, 100, hwnd, (HMENU)IDC_SEND_BUTTON, GetModuleHandle(NULL), NULL));

	vControls.push_back(CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		90, MASTER_HEIGHT - 340, 300, 20, hwnd, (HMENU)IDC_URL_ENTRY, GetModuleHandle(NULL), NULL));

	//HTTP Method radio buttons
	vControls.push_back(CreateWindowExA(NULL, "BUTTON", "GET", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_GROUP | BS_RADIOBUTTON,
		50, 50, RADIO_BTN_WIDTH, RADIO_BTN_HEIGHT, hwnd, (HMENU)IDC_RADIO_GET, GetModuleHandle(NULL), NULL));

	vControls.push_back(CreateWindowExA(NULL, "BUTTON", "POST", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
		50, 80, RADIO_BTN_WIDTH, RADIO_BTN_HEIGHT, hwnd, (HMENU)IDC_RADIO_POST, GetModuleHandle(NULL), NULL));

	vControls.push_back(CreateWindowExA(NULL, "BUTTON", "PUT", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
		50, 110, RADIO_BTN_WIDTH, RADIO_BTN_HEIGHT, hwnd, (HMENU)IDC_RADIO_PUT, GetModuleHandle(NULL), NULL));

	vControls.push_back(CreateWindowExA(NULL, "BUTTON", "PATCH", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
		50, 140, RADIO_BTN_WIDTH, RADIO_BTN_HEIGHT, hwnd, (HMENU)IDC_RADIO_PATCH, GetModuleHandle(NULL), NULL));

	vControls.push_back(CreateWindowExA(NULL, "BUTTON", "DELETE", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
		50, 170, RADIO_BTN_WIDTH, RADIO_BTN_HEIGHT, hwnd, (HMENU)IDC_RADIO_DELETE, GetModuleHandle(NULL), NULL));

	vControls.push_back(CreateWindowExA(NULL, "BUTTON", "HEAD", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
		50, 200, RADIO_BTN_WIDTH, RADIO_BTN_HEIGHT, hwnd, (HMENU)IDC_RADIO_HEAD, GetModuleHandle(NULL), NULL));

	vControls.push_back(CreateWindowExA(NULL, "BUTTON", "OPTIONS", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
		50, 230, RADIO_BTN_WIDTH, RADIO_BTN_HEIGHT, hwnd, (HMENU)IDC_RADIO_OPTIONS, GetModuleHandle(NULL), NULL));

	//Read-only text area for responses
	vControls.push_back(CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "RESPONSE_AREA", WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | ES_READONLY, 
		50, MASTER_HEIGHT - 300, MASTER_WIDTH - 100, 200, hwnd, (HMENU)IDC_RESPONSE_AREA, GetModuleHandle(NULL), NULL));

	std::for_each(vControls.begin(), vControls.end(), [&success](HWND pCtrl) {
		if (pCtrl == NULL) { success = false; };
	});
	if (success) {
		std::for_each(vControls.begin(), vControls.end(), [&hFont](HWND pCtrl) {
			SendMessage(pCtrl, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		});
		HFONT hBold = CreateFont(22, 0, 0, 0, FW_EXTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		SendMessage(vControls[BTN_SEND], WM_SETFONT, (WPARAM)hBold, NULL);
		SendMessageA(vControls[AREA_RESPONSE], WM_SETTEXT, NULL, (LPARAM)"");
	}
	return success;
}

//http://www.cplusplus.com/forum/windows/17963/
void WINAPI paintText(HWND hwnd) {
	HFONT hTmp, hBold;
	PAINTSTRUCT ps;
	HDC hdc;

	hBold = CreateFont(18, 0, 0, 0, FW_EXTRABOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

	hdc = BeginPaint(hwnd, &ps);
	hTmp = (HFONT)SelectObject(hdc, hBold); //Change the font for drawing (in the HDC)
	TextOut(hdc, 50, 30, L"HTTP Method:", 13);
	TextOut(hdc, 50, MASTER_HEIGHT - 340, L"URL:", 5);
	DeleteObject(SelectObject(hdc, hTmp)); //Prevent memory leak
	EndPaint(hwnd, &ps);
}

void WINAPI handleCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//Sends off the HTTP request
	if (gRequestSender->isMatchingID(LOWORD(wParam)) && gCanSend) {

		std::wstring response;
		std::string dstURL;

		gCanSend = false;

		//Ugly but necessary steps to get the value of the URL dialog
		int buffLen = GetWindowTextLength(vControls[URL_ENTRY]);
		char *buff = new char[buffLen+1];
		GetWindowTextA(vControls[URL_ENTRY], buff, buffLen+1);
		dstURL = buff;
		delete buff;

		SendMessageA(vControls[AREA_RESPONSE], WM_SETTEXT, NULL, (LPARAM)"Loading...");
		gRequestSender->sendRequest(dstURL.c_str(), gThreadBool, gThreadResponse, gOptions);
	} else {
		switch (LOWORD(wParam)) {
			case IDC_RADIO_GET:
				handleRadioBtn(hwnd, OPTION_METHOD, "GET", IDC_RADIO_GET);
				break;
			case IDC_RADIO_POST :
				handleRadioBtn(hwnd, OPTION_METHOD, "POST", IDC_RADIO_POST);
				break;
			case IDC_RADIO_PUT:
				handleRadioBtn(hwnd, OPTION_METHOD, "PUT", IDC_RADIO_PUT);
				break;
			case IDC_RADIO_PATCH:
				handleRadioBtn(hwnd, OPTION_METHOD, "PATCH", IDC_RADIO_PATCH);
				break;
			case IDC_RADIO_DELETE:
				handleRadioBtn(hwnd, OPTION_METHOD, "DELETE", IDC_RADIO_DELETE);
				break;
			case IDC_RADIO_HEAD:
				handleRadioBtn(hwnd, OPTION_METHOD, "HEAD", IDC_RADIO_HEAD);
				break;
			case IDC_RADIO_OPTIONS:
				handleRadioBtn(hwnd, OPTION_METHOD, "OPTIONS", IDC_RADIO_OPTIONS);
				break;
			default:
				break;
		}
	}
}

void WINAPI handleRadioBtn(HWND hwnd, int methodID, const char* methodVal, DWORD resourceID) {
	gOptions.setOption(methodID, methodVal);
	CheckRadioButton(hwnd, IDC_RADIO_GET, IDC_RADIO_OPTIONS, resourceID);
}