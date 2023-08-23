#include "world.hpp"

World::World(ID3D10Device* device, std::string name, std::string fxShader) : log("World", name), device(device), name(name) {
    ID3D10Blob* errors = NULL;
    HRESULT hr = D3DX10CreateEffectFromFile(fxShader.c_str(), NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, device, NULL, NULL, &this->effect, &errors, NULL);
    
    if (errors) {
        log << (char*)errors->GetBufferPointer() << std::endl;
    }

    log << Log::Assert(FAILED(hr)) << "Failed to compile effect" << std::endl;

    this->technique = this->effect->GetTechniqueByName("MainRender");

    this->worldVariable = this->effect->GetVariableByName("world")->AsMatrix();
    this->viewVariable = this->effect->GetVariableByName("view")->AsMatrix();
    this->projectionVariable = this->effect->GetVariableByName("projection")->AsMatrix();
}

void World::addObject(Object* obj) {
    obj->prepareVertexLayout(device, this->technique);
    obj->prepareVertexBuffer(device);

    this->objects.push_back(obj);
};

Object* World::getobjectByName(std::string name) {
    for (auto object : this->objects) {
        if (object->name == name)
            return object;
    }

    return NULL;
}

void World::update() {
    for (auto& object : this->objects) {
        object->update();
    }
}

void World::draw(Engine::Camera& cam) {
    ID3D10EffectMatrixVariable* worldInvertedTransposedVar = this->effect->GetVariableByName("worldInvertedTransposed")->AsMatrix();
    ID3D10EffectVectorVariable* cameraPositionVar = this->effect->GetVariableByName("cameraPosition")->AsVector();

    D3DXVECTOR3 camPos = cam.getPos();
    cameraPositionVar->SetFloatVector((float*)&camPos);

    D3DXMATRIX g_View = cam.getViewMatrix();
    D3DXMATRIX g_Proj = cam.getProjectionMatrix();

    this->viewVariable->SetMatrix((float*)&g_View);
    this->projectionVariable->SetMatrix((float*)&g_Proj);
    
    this->lighting.setLights(this->effect);
    D3D10_TECHNIQUE_DESC techDesc;
    this->technique->GetDesc(&techDesc);

    for (auto& object : this->objects) {
        for(UINT p = 0; p < techDesc.Passes; ++p) {
            D3DXMATRIX g_Model = object->getModelMatrix();
            this->worldVariable->SetMatrix((float*)&g_Model);
            D3DXMATRIX worldInverted, worldInvertedTransposed;
            D3DXMatrixIdentity(&worldInverted);
            D3DXMatrixIdentity(&worldInvertedTransposed);
            D3DXMatrixInverse(&worldInverted, nullptr, &g_Model);
            D3DXMatrixTranspose(&worldInvertedTransposed, &worldInverted);
            worldInvertedTransposedVar->SetMatrix((float*)&worldInvertedTransposed);

            this->technique->GetPassByIndex(p)->Apply(0);
            object->draw(device);
        }
    }
}