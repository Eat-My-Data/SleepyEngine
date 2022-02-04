#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Camera.h"
#include "../Drawable/DirectionalLight.h"
#include "../Drawable/PointLight.h"
#include "../Drawable/SpotLight.h"
#include "../Bindable/Bindables/StructuredBuffers.h"
#include "../ResourceManager/Model.h"

class RenderGraph;

class LightManager
{
public:
	void Initialize( GraphicsDeviceInterface& gdi );
	void UpdateBuffers();
	void Draw();
	void RenderLightGeometry();
	void PrepareDepthFromLight();
	void RenderPointLightCubeTextures( const Model& model );
	void PrepareDepthFromSpotLight();
	void DrawControlPanel();
	void LinkTechniques( Rgph::RenderGraph& rg );
public:
	void Submit();
	void SelectPointLight( const u32 index );
	void TranslatePointLight( DirectX::XMFLOAT3 translation );
	void SelectSpotLight( const u32 index );
	void TranslateSpotLight( DirectX::XMFLOAT3 translation );
	void RotateSpotLight( const f32 dx, const f32 dy );
	void TranslateDirectionalLight( DirectX::XMFLOAT3 translation );
	void RotateDirectionalLight( const f32 dx, const f32 dy );
private:
	void InitializePointLightShadowResources();
public:
	DirectionalLight* m_pDirectionalLight;
	Bind::PixelStructuredBuffer<DirectionalLight::DirectionalLightData>* m_pDirectionalLightBuffer;
	SpotLight* m_pSpotLight;
	Bind::PixelStructuredBuffer<SpotLight::SpotLightData>* m_pSpotLightBuffer;
	std::vector<PointLight*> m_vecOfPointLights;
	Bind::PixelArrStructuredBuffer<PointLight::PointLightData>* m_pPointLightBuffer;
private:
	int m_iSelectedPointLight = 0;
	int m_iSelectedSpotLight = 0;
	GraphicsDeviceInterface* m_pGDI = nullptr;
private:
	std::vector<ID3D11ShaderResourceView*> pTextureView;
	ID3D11ShaderResourceView* pTextureView2;
	DirectX::XMFLOAT4X4 projection;
	std::vector<ID3D11DepthStencilView*> depthBuffers{ 6 };
	std::vector<ID3D11DepthStencilView*> depthBuffers2{ 6 };
	std::vector<DirectX::XMFLOAT3> cameraDirections{ 6 };
	std::vector<DirectX::XMFLOAT3> cameraUps{ 6 };
private:
	struct DefaultLightSettings
	{
		DirectX::XMFLOAT3 ambientLight = { 0.2f, 0.2f, 0.2f };
		float specularPower = 128.0f;
		float lightIntensity = 1.0f;
		float padding[3];
	} m_DefaultLightSettings;
	Bind::PixelConstantBuffer<DefaultLightSettings>* m_pDefaultLightSettingsBuffer;

	struct LightIndex
	{
		int index = 0;
		int numPointLights = 2;
		float padding[2];
	} m_LightIndexes;
	Bind::PixelConstantBuffer<LightIndex>* m_pLightIndex;

	struct SolidGeometryColor
	{
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float padding;
	} m_SolidGeometryColor;
	Bind::PixelConstantBuffer<SolidGeometryColor>* m_pSolidGeometryColorBuffer;
	// TODO:
	// - Make start positions available for lights
	// - Add support to turn on/turn off lights
};