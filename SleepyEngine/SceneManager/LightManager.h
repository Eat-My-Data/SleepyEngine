#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Camera.h"
#include "../Drawable/DirectionalLight.h"
#include "../Drawable/PointLight.h"
#include "../Drawable/SpotLight.h"
#include "../Bindable/Bindables/StructuredBuffers.h"
#include "../ResourceManager/Mesh.h"

class LightManager
{
public:
	void Initialize( GraphicsDeviceInterface& gdi );
	void UpdateBuffers( DirectX::XMFLOAT3 camPos );
	void Draw();
	void RenderLightGeometry();
	void PrepareDepthFromLight();
	void RenderPointLightCubeTextures( const Model& model );
	void PrepareDepthFromSpotLight();
	void DrawControlPanel();
public:
	void SelectPointLight( const u32 index );
	void TranslatePointLight( DirectX::XMFLOAT3 translation );
	void SelectSpotLight( const u32 index );
	void TranslateSpotLight( DirectX::XMFLOAT3 translation );
	void RotateSpotLight( const f32 dx, const f32 dy );
	void TranslateDirectionalLight( DirectX::XMFLOAT3 translation );
	void RotateDirectionalLight( const f32 dx, const f32 dy );
	// MAKE PRIVATE
public:
	DirectionalLight* m_pDirectionalLight;
	Bind::PixelStructuredBuffer<DirectionalLight::DirectionalLightData>* m_pDirectionalLightBuffer;
	SpotLight* m_pSpotLight;
	Bind::PixelStructuredBuffer<SpotLight::SpotLightData>* m_pSpotLightBuffer;
	std::vector<PointLight*> m_vecOfPointLights;
	Bind::PixelArrStructuredBuffer<PointLight::PointLightData>* m_pPointLightBuffer;
private:
	u32 m_iSelectedPointLight = 0;
	u32 m_iSelectedSpotLight = 0;
	GraphicsDeviceInterface* m_pGDI = nullptr;
private:
	ID3D11ShaderResourceView* pTextureView;
	ID3D11ShaderResourceView* pTextureView2;
	DirectX::XMFLOAT4X4 projection;
	std::vector<ID3D11DepthStencilView*> depthBuffers{ 6 };
	std::vector<ID3D11DepthStencilView*> depthBuffers2{ 6 };
	std::vector<DirectX::XMFLOAT3> cameraDirections{ 6 };
	std::vector<DirectX::XMFLOAT3> cameraUps{ 6 };
private:
	struct DefaultLightSettings
	{
		float specularPower = 128.0f;
		float ambientLight = 0.2f;
		float lightIntensity = 1.0f;
		float padding;
	};
	Bind::PixelConstantBuffer<DefaultLightSettings>* m_pDefaultLightSettingsBuffer;

	struct LightIndex
	{
		float index = 0;
		float numPointLights = 2;
		float padding[2];
	} m_LightIndexes;
	Bind::PixelConstantBuffer<LightIndex>* m_pLightIndex;
	// TODO:
	// - Make start positions available for lights
	// - Optimize everything you can
	// - Polish
	// - Shadows for Spotlight
	// - Add support to turn on/turn off lights
};