#include "directx.hpp"

namespace Engine {
	namespace Base {

        DirectXDevice::DirectXDevice(int width, int height, LPCSTR name, bool windowed, ID3D10Device** device, IDXGISwapChain** swapChain) : Window(width, height, name), log("DirectX") {
			D3D10_DRIVER_TYPE driverType = D3D10_DRIVER_TYPE_HARDWARE;
			UINT createDeviceFlags = 0;
			// #ifdef _DEBUG
				//createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
			// #endif
			
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			swapChainDesc.BufferCount = 1;
			swapChainDesc.BufferDesc.Width = width;
			swapChainDesc.BufferDesc.Height = height;
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.OutputWindow = hWnd;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.Windowed = windowed;
			
			HRESULT hr = D3D10CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, D3D10_SDK_VERSION, &swapChainDesc, swapChain, device);
			log << Log::Assert(FAILED(hr)) << "Failed create DirectX device " << hr << std::endl;;
		}

		DirectXDevice::~DirectXDevice() {}
	}
}