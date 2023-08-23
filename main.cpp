#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <d3d10.h>
#include <D3DX10.h>

#include "log.hpp"
#include "directx.hpp"
#include "objparser.hpp"
#include "camera.hpp"
#include "world.hpp"

Log mlog("Main Thread");

int lastkey;

ID3D10Device* device = NULL;
IDXGISwapChain* deviceSwapChain = NULL;
Engine::Base::DirectXDevice directx(1280, 720, "Dungeon Slave Engine", TRUE, &device, &deviceSwapChain);

Engine::Camera cam((float)directx.width / (float)directx.height, (float)D3DXToRadian(90.0f), 1000.0f, 0.1f);

ObjParser parser("resources/scene1.obj");
// ObjParser axis("resources/xyz_axis.obj");

ID3DX10Font* Font = NULL;

World mainWorld(device, "main", "resources/effects/first.fx");

ID3D10RasterizerState* rasterizerState;
ID3D10RenderTargetView* pRenderTargetView;
ID3D10DepthStencilState* pDepthStencilState;
ID3D10DepthStencilView* pDepthStencilView;
ID3D10Texture2D* pDepthStencilBuffer;
ID3D10Texture2D* pBackBuffer;

struct screen {
	std::string line;
    screen() {}
	screen(std::string line) : line(line) {}

    RECT toRect(float x, float y, float w, float h) {
        return RECT{ (LONG)x, (LONG)y, (LONG)(x + w), (LONG)(y + h) };
    }

    template<typename T>
    friend struct screen operator<<(struct screen obj, T val) {
        obj.line += val;
        return obj;
    }

	int operator>>(int l) {
        RECT fontRect = toRect((float)10, (float)(10 + (20 * l)), (float)400, (float)18);
        Font->DrawTextA(NULL, line.c_str(), -1, &fontRect, DT_NOCLIP | DT_TOP | DT_LEFT, 0xFFFFEE00);
        return l;
	}
};

LARGE_INTEGER frequency;
LARGE_INTEGER prevTime;
float frameTime;
float fps;

void InitializeTimer() {
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&prevTime);
}

void UpdateTime() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    frameTime = static_cast<float>(currentTime.QuadPart - prevTime.QuadPart) / frequency.QuadPart;
    prevTime = currentTime;

    fps = 1.0f / frameTime;
}

void Render() {
    device->ClearRenderTargetView(pRenderTargetView, D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f));
    device->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

    device->OMSetDepthStencilState(pDepthStencilState, 0);
    mainWorld.draw(cam);

    D3DXVECTOR3 camPos = cam.getPos();

    screen() << std::format("FPS: {} Frame time: {}", fps, frameTime) >> 0;
    screen() << std::format("lastKey: {}", lastkey) >> 1;
    screen() << std::format("Camera position (X: {} Y: {} Z: {})", camPos.x, camPos.y, camPos.z) >> 2;

    deviceSwapChain->Present(0, 0);
}

void Update() {
    static float t = 0.0f;

    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount();
    if( dwTimeStart == 0 )
    dwTimeStart = dwTimeCur;
    t = ( dwTimeCur - dwTimeStart ) / 1000.0f;


    pointLight_t* light0 = mainWorld.lighting.getPointLight(0);

    D3DXMATRIX worldMatrix, positionMatrix, rotationMatrix;
    D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 12.0f, 12.0f);

    D3DXMatrixIdentity(&worldMatrix);
	D3DXMatrixIdentity(&positionMatrix);
	D3DXMatrixIdentity(&rotationMatrix);

    D3DXMatrixTranslation(&positionMatrix, 0.0f, 0.0f, 0.0f);
    D3DXMatrixRotationYawPitchRoll(&rotationMatrix, t, 0.0f, 0.0f);

    D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &rotationMatrix);
    D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &positionMatrix);

    D3DXVECTOR4 resultPos;
    D3DXVec3Transform(&resultPos, &pos, &worldMatrix);
    light0->position.x = resultPos.x;
    light0->position.y = resultPos.y;
    light0->position.z = resultPos.z;

    cam.update(directx.width, directx.height);

    // Object* sphere = mainWorld.getobjectByName("Sphere1");
    // if (sphere) {
    //     sphere->setRotation({ t, 0.0f, 0.0f });
    // }
    mainWorld.update();
}

void ML() {
    UpdateTime();
    Update();
    Render();
} 

int AppMain() {
	mlog << Log::Level::INFO << "Entering to mainloop " << std::endl;
    InitializeTimer();

    HRESULT hr = deviceSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), (LPVOID*)&pBackBuffer );
    mlog << Log::Assert(FAILED(hr)) << "Failed to get Swap Chain buffer" << std::endl;

    hr = device->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
    mlog << Log::Assert(FAILED(hr)) << "Failed to create Render Target" << std::endl;
    pBackBuffer->Release();

    D3D10_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = directx.width;
    depthStencilDesc.Height = directx.height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D10_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    depthStencilViewDesc.Format = depthStencilDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    D3D10_DEPTH_STENCIL_DESC pDepthStateDesc;  
    ZeroMemory(&pDepthStateDesc,sizeof(D3D10_DEPTH_STENCIL_DESC));
    pDepthStateDesc.DepthEnable = true;
    pDepthStateDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
    pDepthStateDesc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
    pDepthStateDesc.StencilEnable = false;

    device->CreateTexture2D(&depthStencilDesc, NULL, &pDepthStencilBuffer);
    device->CreateDepthStencilState( &pDepthStateDesc, &pDepthStencilState );
    device->CreateDepthStencilView(pDepthStencilBuffer, &depthStencilViewDesc, &pDepthStencilView);

    device->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

    D3D10_VIEWPORT vp;
    vp.Width = directx.width;
    vp.Height = directx.height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    device->RSSetViewports(1, &vp);

    D3D10_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D10_RASTERIZER_DESC));
    rasterizerDesc.FillMode = D3D10_FILL_SOLID;
    rasterizerDesc.CullMode = D3D10_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0;
    rasterizerDesc.SlopeScaledDepthBias = 0;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.MultisampleEnable = TRUE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;

    device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    device->RSSetState(rasterizerState);

    D3DX10_FONT_DESC FontDesc;
	FontDesc.Height         = 20;
	FontDesc.Width          = 0;
	FontDesc.Weight         = 0;
	FontDesc.MipLevels      = 1;
	FontDesc.Italic         = false;
	FontDesc.CharSet        = OUT_DEFAULT_PRECIS;
	FontDesc.Quality        = DEFAULT_QUALITY;
	FontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    strcpy(FontDesc.FaceName, "Arial");
    D3DX10CreateFontIndirect(device, &FontDesc, &Font);

    pointLight_t* light0 = mainWorld.lighting.getPointLight(0);

    light0->flags |= LIGHT_FLAG_ENABLED;

    light0->position = D3DXVECTOR3(0.0, 0.0, 0.0);

    light0->amblient = D3DXVECTOR3(1.0, 1.0, 1.0);
    light0->diffuse = D3DXVECTOR3(1.0, 1.0, 1.0);
    light0->specular = D3DXVECTOR3(1.0, 1.0, 1.0);

    light0->specularShininess = 1.0f;
    light0->constantAttenuation = 1.0f;
    light0->linearAttenuation = 0.09f;
    light0->quadraticAttenuation = 0.032f;

    // cam.setPos();
    // cam.setSpeed(10.0f);

    for (auto& object : parser.objects) {
        // object.setPosition({3.0f, 3.0f, 10.0f});
        mainWorld.addObject(&object);
    }

    // for (auto& object : axis.objects) {
    //     mainWorld.addObject(&object);
    // }

	directx.MainLoop(ML);
	mlog << Log::Level::INFO << "Leaving mainloop " << std::endl;
	return 0;
}

void EventHandler(UINT message, WPARAM wParam, LPARAM iParam) {
	switch (message) {
        case WM_DESTROY:
            device->Release();
            PostQuitMessage(0);
            break;
        case WM_CHAR:
            lastkey = (int)wParam;
            switch (wParam) {
                case 'w':
                case 'W':
                case 246:
                case 214:
                    cam.moveForward();
                    break;
                case 's':
                case 'S':
                case 251:
                case 219:
                    cam.moveBack();
                    break;
                case 'a':
                case 'A':
                case 244:
                case 212:
                    cam.moveLeft();
                    break;
                case 'd':
                case 'D':
                case 226:
                case 194:
                    cam.moveRight();
                    break;
                case 'g':
                case 'G':
                case 239:
                case 207:
                    cam.grab_ungrabCursor();
                    break;
                case VK_ESCAPE:
                    device->Release();
                    PostQuitMessage(0);
                    break;
                default:
                    break;
            }
            break;
        case WM_SETCURSOR:
            if (cam.isGrabbed)
                SetCursor(NULL);
            else
                SetCursor(LoadCursor(NULL, IDC_ARROW));
            break;
	}
}

int main() {
	mlog << Log::Level::INFO << "Entering to main entrypoint" << std::endl;

	directx.addEventHandler(EventHandler);
	AppMain();

	mlog << Log::Level::INFO << "Shutting down " << std::endl;
	exit(0);
}