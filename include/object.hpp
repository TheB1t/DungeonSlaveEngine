#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <d3d10.h>
#include <D3DX10.h>

#include "log.hpp"

class Object {
    public:
        typedef D3DXVECTOR4 Vertex_t;
        typedef D3DXVECTOR3 Normal_t;
        typedef D3DXVECTOR2 TexCoord_t;

        typedef struct {
            Vertex_t vertex;
            Normal_t normal;
            D3DXCOLOR color;
        } PackedVertex_t;

        typedef struct {
            DWORD vertexIndex;
            DWORD normalIndex;
            DWORD texCoordIndex;
        } Face_t;

        typedef struct {
            Face_t faces[3];
        } Trangle_t;

        std::string name;

        Object(std::string name);
        ~Object();

		void setPosition(D3DXVECTOR3 vec);
		void setRotation(D3DXVECTOR3 vec);

        void addVertex(Vertex_t& vertex);
        void addNormal(Normal_t& normal);
        void addTexCoord(TexCoord_t& texCoord);
        void addTrangle(Trangle_t& trangle);

        int getVertexCount();
        int getNormalCount();
        int getTexCoordCount();
        int getTrangleCount();

        D3DXMATRIX getModelMatrix();

        void pack();
        
        void prepareVertexLayout(ID3D10Device* device, ID3D10EffectTechnique* technique);
        void prepareVertexBuffer(ID3D10Device* device);

        void update();
        void draw(ID3D10Device* device);

    private:
        Log log;
        
        std::vector<PackedVertex_t> packedVertices;
        std::vector<DWORD> packedIndices;

        std::vector<Vertex_t> vertices;
        std::vector<Normal_t> normals;
        std::vector<TexCoord_t> texCoords;
        std::vector<Trangle_t> trangles;

        ID3D10Buffer* vertexBuffer = NULL;
        ID3D10Buffer* indexBuffer = NULL;
        ID3D10InputLayout* vertexLayout = NULL;

		D3DXVECTOR3 position = D3DXVECTOR3{ 0.0f, 0.0f, 0.0f };
		D3DXVECTOR3 rotation = D3DXVECTOR3{ 0.0f, 0.0f, 0.0f };
        D3DXVECTOR3 scale = D3DXVECTOR3{ 1.0f, 1.0f, 1.0f };

		D3DXMATRIX modelMatrix;
		D3DXMATRIX positionMatrix;
		D3DXMATRIX rotationMatrix;
        D3DXMATRIX scaleMatrix;
};