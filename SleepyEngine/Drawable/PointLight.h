#pragma once
#include "./Drawable.h"
#include "../SceneManager/RenderTechnique.h"
#include "../Drawable/SolidSphere.h"

class PointLight : public Drawable
{
public:
	PointLight( GraphicsDeviceInterface& gdi, float radius );
	void Draw( GraphicsDeviceInterface& gdi );
	void Update( DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, DirectX::XMFLOAT3 camPos );
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Translate( DirectX::XMFLOAT3 vec );
	bool CameraIsInside( DirectX::XMFLOAT3 camPos );
public:
	struct PointLightData
	{
		DirectX::XMFLOAT3 pos = { 10.0f, 9.0f, 2.5f };
		float specularPower = 128.0f;
		DirectX::XMFLOAT3 ambient = { 0.2f, 0.2f, 0.2f };
		float diffuseIntensity = 1.0f;
		DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
		float attConst = 1.0f;
		float attQuad = 0.0075f;
		float attLin = 0.045f;
		float padding[2];
		DirectX::XMFLOAT3 camPos;
		float radius = 5.0f;
		DirectX::XMMATRIX cameraMatrix;
		DirectX::XMMATRIX projInvMatrix;
	};
	PointLightData m_StructuredBufferData;
private:
	SolidSphere* m_SolidSphere;
private:
	ID3D11PixelShader* pPixelShader;
	ID3D11RasterizerState* rasterizerInside;
	ID3D11RasterizerState* rasterizerOutside;
	ID3D11DepthStencilState* pDSStateInfrontBackFaceOfLight;
	ID3D11DepthStencilState* pDSStateLightingBehindFrontFaceOfLight;
	ID3D11DepthStencilState* pDSStateInsideLighting;
};
