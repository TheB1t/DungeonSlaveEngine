#include "light.hpp"

Lighting::Lighting() : log("Lighting") {
    ZeroMemory(this->pointLights, sizeof(pointLight_t) * MAX_POINT_LIGHTS);
}

pointLight_t* Lighting::getPointLight(int index) {
    if (index >= MAX_POINT_LIGHTS)
        return NULL;

    return &this->pointLights[index];
}

pointLight_t* Lighting::getPointLights() {
    return this->pointLights;
}

void Lighting::setLights(ID3D10Effect* effect) {
    ID3D10EffectVariable* pointLightsVar = effect->GetVariableByName("pointLights");
    pointLightsVar->SetRawValue(this->pointLights, 0, sizeof(pointLight_t) * MAX_POINT_LIGHTS);
}