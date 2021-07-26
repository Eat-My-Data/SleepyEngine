#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Camera.h"
#include "../Drawable/DirectionalLight.h"
#include "../Drawable/PointLight.h"

class LightManager
{
public:
	void Initialize( GraphicsDeviceInterface& gdi );
	void UpdateBuffers( DirectX::XMFLOAT3 camPos );
	void Draw();
	void PrepareDepthFromLight();
public:
	void TranslatePointLight( DirectX::XMFLOAT3 translation );
	void TranslateDirectionalLight( DirectX::XMFLOAT3 translation );
	void RotateDirectionalLight( const f32 dx, const f32 dy );
private:
	DirectionalLight* m_pDirectionalLight;
	std::vector<PointLight*> m_vecOfPointLights;
private:
	GraphicsDeviceInterface* m_pGDI = nullptr;
};