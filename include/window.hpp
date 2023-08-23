#pragma once

#include <Windows.h>
#include <vector>
#include <tchar.h>

#include "log.hpp"

namespace Engine {
	namespace Base {
        typedef void (*eventHandler_t)(UINT message, WPARAM wParam, LPARAM iParam);
        typedef void (*loopMain_t)(void);

		class Window {
			public:
				int width;
				int height;
				Window(int width, int height, LPCSTR name);
				void MainLoop(loopMain_t otherfunc);
				void addEventHandler(eventHandler_t handler);
				~Window() {}

			private:
				Log log;

			protected:
				HWND hWnd;
		};
    }
}