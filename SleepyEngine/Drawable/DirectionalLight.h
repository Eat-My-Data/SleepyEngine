#pragma once
#include "Drawable.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../Camera/Camera.h"
#include "../Utilities/ChiliMath.h"
#include "../Drawable/DeferredFullScreen.h"

class DirectionalLight
{
public:
	DirectionalLight( Graphics& gfx );
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Submit( size_t channels ) const noxnd;
	void Bind( Graphics& gfx, DirectX::FXMMATRIX view ) const noexcept;
	void LinkTechniques( Rgph::RenderGraph& rg );
	std::shared_ptr<Camera> ShareCamera() const noexcept;
	void ToggleRenderTechnique( Graphics& gfx, const std::string& renderTechnique );
private:
	struct DirectionalLightCBuf
	{
		DirectX::XMFLOAT3 lightDirection;
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT3 ambient;
		float padding[3];
		DirectX::XMMATRIX lightViewProjectionMatrix;
	};
	DirectionalLightCBuf home;
	DirectionalLightCBuf cbData;
	mutable DeferredFullScreen mesh;
	mutable Bind::PixelConstantBuffer<DirectionalLightCBuf> cbuf;
	std::shared_ptr<Camera> pCamera;
//private:
//	struct ForwardMatrices
//	{
//		DirectX::XMMATRIX lightViewMatrix;
//		DirectX::XMMATRIX lightProjMatrix;
//	} matrixcbuf;
//	std::shared_ptr<Bind::VertexConstantBuffer<ForwardMatrices>> m_pForwardLightMatrices;
};
