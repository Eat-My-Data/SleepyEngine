#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Camera.h"
#include "../Drawable/DirectionalLight.h"
#include "../Drawable/PointLight.h"
#include "../Bindable/Bindables/StructuredBuffers.h"

class LightManager
{
public:
	void Initialize( GraphicsDeviceInterface& gdi );
	void UpdateBuffers( DirectX::XMFLOAT3 camPos );
	void Draw();
	void RenderSolidSpheres();
	void PrepareDepthFromLight();
public:
	void SelectLight( const u32 index );
	void TranslatePointLight( DirectX::XMFLOAT3 translation );
	void TranslateDirectionalLight( DirectX::XMFLOAT3 translation );
	void RotateDirectionalLight( const f32 dx, const f32 dy );
private:
	DirectionalLight* m_pDirectionalLight;
	Bind::PixelStructuredBuffer<DirectionalLight::DirectionalLightData>* m_pPixelStructuredBuffer;
	std::vector<PointLight*> m_vecOfPointLights;
	Bind::PixelArrStructuredBuffer<PointLight::PointLightData>* m_pPixelArrStructuredBuffer;
private:
	u32 m_iSelectedLight = 0;
	GraphicsDeviceInterface* m_pGDI = nullptr;
};