#pragma once

#include <sstream>
#include <vector>
#include <string>
#include <d3d10.h>
#include <D3DX10.h>

namespace Engine {
	class Camera {
        public:
            bool isGrabbed = false;
            Camera(float aspectRatio, float yfov, float zfar, float znear);
            void setSpeed(float speed);
            void moveForward();
            void moveBack();
            void moveLeft();
            void moveRight();
            void setDir(float angleX, float angleY);
            void setPos(float x, float y, float z);
            D3DXVECTOR3 getDir();
            D3DXVECTOR3 getPos();
            D3DXMATRIX getViewMatrix();
            D3DXMATRIX getProjectionMatrix();
            void grab_ungrabCursor();
            void update(int w, int h);

        private:
            float aspectRatio, yfov, zfar, znear;
            
            D3DXVECTOR3 m_Position;
            D3DXVECTOR3 m_Target;
            D3DXVECTOR3 m_Up;
            
            D3DXVECTOR3 m_Front;
            D3DXVECTOR3 m_Right;

            D3DXMATRIX m_ViewMatrix;
            D3DXMATRIX m_ProjectionMatrix;

            POINT m_PrevCursorPos;
            float m_Yaw = 0.0f;
            float m_Pitch = 0.0f;

            float speed = 0.1f;
	};
}