#pragma once

#include "window.hpp"

namespace Engine {
	namespace Base {
		std::vector<eventHandler_t> eventHandlers;

		LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM iParam) {
			for (auto Event : eventHandlers)
				Event(message, wParam, iParam);
			return (DefWindowProc(hWnd, message, wParam, iParam));
		}

		Window::Window(int width, int height, LPCSTR name) : log("Window"), width(width), height(height) {
			WNDCLASS wcl;
			wcl.hInstance = GetModuleHandleW(NULL);
			wcl.lpszClassName = "MainWindow";
			wcl.lpfnWndProc = WindowProc;
			wcl.style = 0;
			wcl.hIcon = LoadIcon(wcl.hInstance, IDC_ICON);
			wcl.hCursor = LoadCursor(wcl.hInstance, IDC_ARROW);
			wcl.lpszMenuName = NULL;
			wcl.cbClsExtra = 0;
			wcl.cbWndExtra = 0;
			wcl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
			RegisterClass(&wcl);
			//hWnd = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE, L"MainWindow", this->name, WS_TILEDWINDOW | WS_VISIBLE, 0, 0, this->width, this->height, NULL, NULL, hThisInst, NULL);
			hWnd = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE, "MainWindow", name, NULL, 0, 0, width, height, NULL, NULL, wcl.hInstance, NULL);
			log << Log::Assert(!hWnd)  << "Window dosen't created" << std::endl;;
			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
		}

		void Window::MainLoop(loopMain_t otherfunc) {
			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT) {
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				} else {
					otherfunc();
				}
			}
		}

		void Window::addEventHandler(eventHandler_t handler) {
			eventHandlers.push_back(handler);
		}
	}
}