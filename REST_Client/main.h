#ifndef __MAG_PROJECT_MAIN_H__
#define __MAG_PROJECT_MAIN_H__

#include <iostream>
#include <vector>
#include <algorithm>
#include "constants.h"
#include "RequestSender.h"

WNDCLASSEX wc {
	wc.cbSize = sizeof(WNDCLASSEX),
	wc.style = 0,
	wc.lpfnWndProc = NULL,
	wc.cbClsExtra = 0,
	wc.cbWndExtra = 0,
	wc.hInstance = NULL,
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION),
	wc.hCursor = LoadCursor(NULL, IDC_ARROW),
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
	wc.lpszMenuName = NULL,
	wc.lpszClassName = L"REST_Client",
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION),
};

bool gCanSend = true; //Locks the send button from when it is pressed to when the dispatched thread returns

std::vector<HWND> vControls;

RequestSender* gRequestSender;
OptionHandler gOptions;

WSADATA wsaData;

HGDIOBJ hDefaultFont;

std::atomic<bool> gThreadBool = false;
std::wstring gThreadResponse;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool WINAPI setup_window(HWND hwnd, HGDIOBJ hFont);
void WINAPI handleCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void WINAPI paintText(HWND hwnd, HGDIOBJ hFont);

#endif