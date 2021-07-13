#pragma once
#include "./Drawable.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../SceneManager/RenderTechnique.h"

class PointLight : public Drawable
{
public:
	PointLight( GraphicsDeviceInterface& gdi, float radius, RenderTechnique renderTechnique );
	void Draw( GraphicsDeviceInterface& gdi, DirectX::XMFLOAT3 camPos );
	void UpdateCBuffers( GraphicsDeviceInterface& gdi, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, DirectX::XMFLOAT3 camPos );
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Translate( DirectX::XMFLOAT3 vec );
	bool CameraIsInside( DirectX::XMFLOAT3 camPos );
	void UpdateForwardCBuffer( GraphicsDeviceInterface& gdi );
private:
	RenderTechnique m_RenderTechnique;
private:
	struct ForwardCBufData
	{
		alignas( 16 ) DirectX::XMFLOAT3 pos = { 10.0f, 9.0f, 2.5f };
		alignas( 16 ) DirectX::XMFLOAT3 ambient = { 0.2f, 0.2f, 0.2f };
		alignas( 16 ) DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
		float diffuseIntensity = 1.0f;
		float attConst = 1.0f;
		float attLin = 0.045f;
		float attQuad = 0.0075f;
	};
	ForwardCBufData m_ForwardCBufData;
	mutable Bind::PixelConstantBuffer<ForwardCBufData> m_pFCbuf;
private:
	// deferred resources
	struct PSColorConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float radius;

		float attConst = 0.0f;
		float attLin = 0.045f;
		float attQuad = 0.0075f;
		float specularPower = 128.0f;

		float diffuseIntensity = 1.0f;
		float specularIntensity = 1.0f;

		DirectX::XMMATRIX cameraMatrix;
		DirectX::XMMATRIX projInvMatrix;
	} colorConst;
	std::shared_ptr<Bind::PixelConstantBuffer<PSColorConstant>> pcs;

	struct PSPositionConstant
	{
		DirectX::XMFLOAT3 lightPosition = { 1.0f,1.0f,1.0f };
		float padding;
	} posConst;
	std::shared_ptr<Bind::PixelConstantBuffer<PSPositionConstant>> pcs2;

	struct CamPosBuffer
	{
		DirectX::XMFLOAT3 camPos;
		float padding2;
	} cambuf;
	std::shared_ptr<Bind::PixelConstantBuffer<CamPosBuffer>> pcs3;

	ID3D11PixelShader* pPixelShader;

	ID3D11RasterizerState* rasterizerInside;
	ID3D11RasterizerState* rasterizerOutside;

	ID3D11DepthStencilState* pDSStateInfrontBackFaceOfLight;
	ID3D11DepthStencilState* pDSStateLightingBehindFrontFaceOfLight;
	ID3D11DepthStencilState* pDSStateInsideLighting;
};
