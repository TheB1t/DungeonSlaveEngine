#pragma once

#include <Windows.h>
#include <d3d10.h>
#include <D3DX10.h>

#include "log.hpp"
#include "window.hpp"

namespace Engine {
	namespace Base {
		class DirectXDevice : public Window {
			public:
				DirectXDevice(int width, int height, LPCSTR name, bool windowed, ID3D10Device** device, IDXGISwapChain** swapChain);
				~DirectXDevice();

			private:
				Log log;
		};
	}
}