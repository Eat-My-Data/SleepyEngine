#pragma once
#include "Drawable.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../SceneManager/RenderTechnique.h"
#include "../SceneManager/Camera.h"

class DirectionalLight : public Drawable
{
public:
	DirectionalLight( GraphicsDeviceInterface& gdi );
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void UpdateCBuffers( GraphicsDeviceInterface& gdi, DirectX::XMFLOAT3 camPos );
	void Draw( GraphicsDeviceInterface& gdi ) const noexcept;
public:
	void Translate( DirectX::XMFLOAT3 translation );
	void Rotate( const f32 dx, const f32 dy );
	DirectX::XMMATRIX GetViewMatrix() noexcept;
	DirectX::XMMATRIX GetProjectionMatrix() noexcept;
	void UpdateForwardCBuffer( GraphicsDeviceInterface& gdi );
private:
	Camera m_OrthoCamera = { L"Directional Light Camera", MatrixType::Orthographic, ViewSpace( 400.0f, 400.0f, 1.0f, 1200.0f ), DirectX::XMFLOAT3{ 0.0f,200.8f,100.0f },  PI / 2.0f, -PI };
private:
	struct DeferredLightBuffer
	{
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f };
		float specularIntensity = 1.0f;
		float att = 0.5f;
		float specularPower = 128.0f;
		DirectX::XMFLOAT3 camPos;
		DirectX::XMMATRIX cameraMatrix;
		DirectX::XMMATRIX projInvMatrix;
		DirectX::XMMATRIX lightViewProjectionMatrix;
	} lbuf;
	std::shared_ptr<Bind::PixelConstantBuffer<DeferredLightBuffer>> m_pDefferedLightPCbuf;
	struct CamBuffer
	{
		DirectX::XMFLOAT3 camPos;
		float padding;
		DirectX::XMMATRIX lightViewProjectionMatrix;
	} cambuf;
	std::shared_ptr<Bind::PixelConstantBuffer<CamBuffer>> m_pCameraPCBuf;
private:
	struct ForwardLightBuffer
	{
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f };
		float padding;
	} dlcbuf;
	std::shared_ptr<Bind::PixelConstantBuffer<ForwardLightBuffer>> m_pForwardLightPCbuf;
	struct ForwaredMatrices
	{
		DirectX::XMMATRIX lightViewMatrix;
		DirectX::XMMATRIX lightProjMatrix;
	} matrixcbuf;
	std::shared_ptr<Bind::VertexConstantBuffer<ForwaredMatrices>> m_pForwardLightMatrices;
};
