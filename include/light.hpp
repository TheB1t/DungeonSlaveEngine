#pragma once

#define MAX_POINT_LIGHTS 16

#define LIGHT_FLAG_ENABLED 0x1

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <d3d10.h>
#include <D3DX10.h>

#include "log.hpp"

typedef struct {
    D3DXVECTOR3 position;
    float specularShininess;
    D3DXVECTOR3 amblient;
    float constantAttenuation;
    D3DXVECTOR3 diffuse;
    float linearAttenuation;
    D3DXVECTOR3 specular;   
    float quadraticAttenuation;
    
    float pad0[3];
    int flags;
} pointLight_t;

class Lighting {
    public:
        Lighting();

        pointLight_t* getPointLight(int index);
        pointLight_t* getPointLights();

        void setLights(ID3D10Effect* effect);

    private:
        Log log;

        pointLight_t pointLights[MAX_POINT_LIGHTS];
};