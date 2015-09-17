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

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;

	if (!RegisterClassEx(&wc))
	{
		MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(0, wc.lpszClassName, L"REST Client", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
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
			if (gThreadBool) {
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
			};
			break;
		case WM_PAINT:
			paintText(hwnd, hDefaultFont);
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
		MASTER_WIDTH - 200, 30, 100, 100, hwnd, (HMENU)IDC_SEND_BUTTON, GetModuleHandle(NULL), NULL));

	vControls.push_back(CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
		90, MASTER_HEIGHT - 400, 300, 20, hwnd, (HMENU)IDC_URL_ENTRY, GetModuleHandle(NULL), NULL));

	vControls.push_back(CreateWindowExA(NULL, "BUTTON", "GET", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
		10, 10, 200, 80, hwnd, (HMENU)IDC_RADIO_GET, GetModuleHandle(NULL), NULL));

	vControls.push_back(CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "RESPONSE_AREA", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL 
		| ES_READONLY, 50, MASTER_HEIGHT - 300, MASTER_WIDTH - 100, 200, hwnd, 
		(HMENU)IDC_RESPONSE_AREA, GetModuleHandle(NULL), NULL));

	std::for_each(vControls.begin(), vControls.end(), [&success](HWND pCtrl) {
		if (pCtrl == NULL) { success = false; };
	});
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
	if (success) {
		std::for_each(vControls.begin(), vControls.end(), [&hFont](HWND pCtrl) {
			SendMessage(pCtrl, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		});
		SendMessageA(vControls[AREA_RESPONSE], WM_SETTEXT, NULL, (LPARAM)"");
	}
	return success;
}

//http://www.cplusplus.com/forum/windows/17963/
void WINAPI paintText(HWND hwnd, HGDIOBJ hFont) {
	HFONT hTmp;
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(hwnd, &ps);
	hTmp = (HFONT)SelectObject(hdc, hFont); //Change the font for drawing (in the HDC)
	TextOut(hdc, 50, MASTER_HEIGHT - 400, L"URL:", 5);
	DeleteObject(SelectObject(hdc, hTmp)); //Prevent memory leak
	EndPaint(hwnd, &ps);
}

void WINAPI handleCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
	}
}