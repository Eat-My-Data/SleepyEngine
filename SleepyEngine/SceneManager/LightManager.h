#pragma once
#include "../GraphicsDeviceInterface/GraphicsDeviceInterface.h"
#include "Camera.h"
#include "../Drawable/DirectionalLight.h"
#include "../Drawable/PointLight.h"
#include "../Bindable/Bindables/StructuredBuffers.h"
#include "../ResourceManager/Mesh.h"

class LightManager
{
public:
	void Initialize( GraphicsDeviceInterface& gdi );
	void UpdateBuffers( DirectX::XMFLOAT3 camPos );
	void Draw();
	void RenderSolidSpheres();
	void PrepareDepthFromLight();
	void RenderPointLightCubeTextures( const Model& model );
	void DrawControlPanel();
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
private:
	ID3D11ShaderResourceView* pTextureView;
	ID3D11ShaderResourceView* pTextureView2;
	DirectX::XMFLOAT4X4 projection;
	std::vector<ID3D11DepthStencilView*> depthBuffers{ 6 };
	std::vector<ID3D11DepthStencilView*> depthBuffers2{ 6 };
	std::vector<DirectX::XMFLOAT3> cameraDirections{ 6 };
	std::vector<DirectX::XMFLOAT3> cameraUps{ 6 };
	struct LightIndex
	{
		float index = 0;
		float numPointLights = 2;
		float padding[2];
	} m_LightIndexes;
	Bind::PixelConstantBuffer<LightIndex>* m_pLightIndex;
	// TODO:
	// - Shadows for Point Light
	// - Spotlight
	// - Shadows for Spotlight
	//   - Will need to separate PrepareDepthFromLight and point lights paths, maybe a GetPointLights() function?
};