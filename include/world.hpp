#pragma once

#include <string>
#include <d3d10.h>
#include <D3DX10.h>

#include "log.hpp"
#include "object.hpp"
#include "camera.hpp"
#include "light.hpp"

class World {
    public:
        Lighting lighting;

        World(ID3D10Device* device, std::string name, std::string fxShader);

        void addObject(Object* obj);
        Object* getobjectByName(std::string name);

        void update();
        void draw(Engine::Camera& camera);

    private:
        Log log;

        std::string name;

        std::vector<Object*> objects;

        ID3D10Device* device;

        ID3D10Effect* effect = NULL;
        ID3D10EffectTechnique* technique = NULL;

        ID3D10EffectMatrixVariable* worldVariable = NULL;
        ID3D10EffectMatrixVariable* viewVariable = NULL;
        ID3D10EffectMatrixVariable* projectionVariable = NULL;
};