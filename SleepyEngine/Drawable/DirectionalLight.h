#pragma once
#include "Drawable.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../SceneManager/RenderTechnique.h"

class DirectionalLight : public Drawable
{
public:
	DirectionalLight( GraphicsDeviceInterface& gdi, RenderTechnique renderTechnique );
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void UpdateCBuffers( GraphicsDeviceInterface& gdi, DirectX::XMMATRIX lightViewMatrix, DirectX::XMMATRIX lightProjectionMatrix, DirectX::XMFLOAT3 camPos );
	void Draw( GraphicsDeviceInterface& gdi ) const noexcept;
private:
	struct DeferredLightBuffer
	{
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f };
		float specularIntensity = 1.0f;
		float att = 0.5f;
		float specularPower = 128.0f;
		float padding[2];
		DirectX::XMMATRIX cameraMatrix;
		DirectX::XMMATRIX projInvMatrix;
	} lbuf;
	std::shared_ptr<Bind::PixelConstantBuffer<DeferredLightBuffer>> m_pDefferedLightPCbuf;
	struct CamBuffer
	{
		DirectX::XMFLOAT3 camPos;
		float padding;
		DirectX::XMMATRIX lightViewMatrix;
		DirectX::XMMATRIX lightProjMatrix;
	} cambuf;
	std::shared_ptr<Bind::PixelConstantBuffer<CamBuffer>> m_pCameraPCBuf;
	struct ForwardLightBuffer
	{
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f };
		float padding;
	} dlcbuf;
	std::shared_ptr<Bind::PixelConstantBuffer<ForwardLightBuffer>> m_pForwardLightPCbuf;
};
