#pragma once
#include "Drawable.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../SceneManager/RenderTechnique.h"
#include "../Camera/Camera.h"
#include "../Drawable/SolidCone.h"

class SpotLight : public Drawable
{
public:
	SpotLight( Graphics& gdi, f32 scale );
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Update( Graphics& gdi );
	void DrawControlPanel();
	void Draw( Graphics& gdi );
	void Submit();
public:
	void Translate( DirectX::XMFLOAT3 translation );
	void Rotate( const f32 dx, const f32 dy );
	DirectX::XMMATRIX GetViewMatrix() noexcept;
	DirectX::XMMATRIX GetProjectionMatrix() noexcept;
	bool CameraIsInside( DirectX::XMFLOAT3 camPos );
public:
	f32 m_fPitch;
	f32 m_fYaw;
public:
	struct SpotLightData
	{
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f };
		float range = 50.0f;
		DirectX::XMFLOAT3 pos = { 0.0f, 10.0f, 0.0f };
		float outerRadius = 0.885f;
		float innerRadius = 0.955f;
		DirectX::XMMATRIX spotViewProjectionMatrix;
	};
	SpotLightData m_StructuredBufferData;
public:
	SolidCone* m_pSolidCone;
private:
	struct ForwardMatrices
	{
		DirectX::XMMATRIX lightViewMatrix;
		DirectX::XMMATRIX lightProjMatrix;
	} matrixcbuf;
	std::shared_ptr<Bind::VertexConstantBuffer<ForwardMatrices>> m_pForwardLightMatrices;
private:
	ID3D11PixelShader* pPixelShader;
	ID3D11RasterizerState* rasterizerInside;
	ID3D11RasterizerState* rasterizerOutside;
	ID3D11DepthStencilState* pDSStateInfrontBackFaceOfLight;
	ID3D11DepthStencilState* pDSStateLightingBehindFrontFaceOfLight;
	ID3D11DepthStencilState* pDSStateInsideLighting;
	// TODO:
	// - draw look at vector for debugging
};
