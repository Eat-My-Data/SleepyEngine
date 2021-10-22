#pragma once
#include "./Drawable.h"
#include "../SceneManager/RenderTechnique.h"
#include "../Drawable/SolidSphere.h"

class PointLight : public Drawable
{
public:
	PointLight( GraphicsDeviceInterface& gdi, float radius );
	void Draw( GraphicsDeviceInterface& gdi );
	void Update();
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Translate( DirectX::XMFLOAT3 vec );
	bool CameraIsInside( DirectX::XMFLOAT3 camPos );
public:
	struct PointLightData
	{
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 pos = { 10.0f, 9.0f, 2.5f };
		float radius = 20.0f;
		float padding;
	};
	PointLightData m_StructuredBufferData;
public:
	SolidSphere* m_SolidSphere;
private:
	ID3D11PixelShader* pPixelShader;
	ID3D11RasterizerState* rasterizerInside;
	ID3D11RasterizerState* rasterizerOutside;
	ID3D11DepthStencilState* pDSStateInfrontBackFaceOfLight;
	ID3D11DepthStencilState* pDSStateLightingBehindFrontFaceOfLight;
	ID3D11DepthStencilState* pDSStateInsideLighting;
};
