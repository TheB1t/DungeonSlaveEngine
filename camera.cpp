#include "camera.hpp"

namespace Engine {
    Camera::Camera(float aspectRatio, float yfov, float zfar, float znear) : aspectRatio(aspectRatio), yfov(yfov), zfar(zfar), znear(znear) {
        m_Position = D3DXVECTOR3(0.0f, 0.0f, -5.0f);
        m_Target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

        D3DXMatrixLookAtLH(&m_ViewMatrix, &m_Position, &m_Target, &m_Up);
        D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix, yfov, aspectRatio, znear, zfar);
    };

	void Camera::setSpeed(float speed) { 
		this->speed = speed; 
	}
	
    void Camera::moveForward() { 
		m_Position += speed * m_Front; 
	}
	
    void Camera::moveBack() { 
		m_Position -= speed * m_Front; 
	}
	
    void Camera::moveLeft() { 
		m_Position -= speed * m_Right; 
	}
	
    void Camera::moveRight() { 
		m_Position += speed * m_Right; 
	}
	
    void Camera::setDir(float angleX, float angleY) { 
		this->m_Pitch = (float)D3DXToRadian(angleX);
		this->m_Yaw = (float)D3DXToRadian(angleY); 
	}
	
    void Camera::setPos(float x, float y, float z) { 
		m_Position = D3DXVECTOR3{ x, y, z }; 
	}
	
    D3DXVECTOR3 Camera::getDir() { 
		return m_Target; 
	}
	
    D3DXVECTOR3 Camera::getPos() { 
		return m_Position; 
	}
	
    D3DXMATRIX Camera::getViewMatrix() { 
		return m_ViewMatrix;
	}
	
    D3DXMATRIX Camera::getProjectionMatrix() { 
		return m_ProjectionMatrix;
	}
	
    void Camera::grab_ungrabCursor() { 
		isGrabbed = !isGrabbed; 
	}
	
    void Camera::update(int w, int h) {
		if (isGrabbed) {
			float centerX = (float)w / 2;
			float centerY = (float)h / 2;

			POINT cursorPos;
			GetCursorPos(&cursorPos);

			float dx = static_cast<float>(cursorPos.x - (int)centerX);
			float dy = static_cast<float>(cursorPos.y - (int)centerY);

			m_Yaw -= dx * 0.1f;
			m_Pitch -= dy * 0.1f;

			// m_Pitch = max(min(m_Pitch, D3DX_PI / 2.0f - 0.01f), -D3DX_PI / 2.0f + 0.01f);

			if (m_Pitch > 89.0f)
				m_Pitch = 89.0f;
			if (m_Pitch < -89.0f)
				m_Pitch = -89.0f;

			m_Front.x = (float)(cos(D3DXToRadian(m_Yaw)) * cos(D3DXToRadian(m_Pitch)));
			m_Front.y = (float)sin(D3DXToRadian(m_Pitch));
			m_Front.z = (float)(sin(D3DXToRadian(m_Yaw)) * cos(D3DXToRadian(m_Pitch)));
			D3DXVec3Normalize(&m_Front, &m_Front);

    		D3DXVec3Cross(&m_Right, &m_Up, &m_Front);
    		D3DXVec3Normalize(&m_Right, &m_Right);

			m_Target = m_Position + m_Front;

			D3DXMatrixLookAtLH(&m_ViewMatrix, &m_Position, &m_Target, &m_Up);

			SetCursorPos((int)centerX, (int)centerY);
		}
	}
}