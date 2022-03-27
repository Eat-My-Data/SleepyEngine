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
		alignas( 16 ) DirectX::XMFLOAT3 lightDirection;
		alignas( 16 ) DirectX::XMFLOAT3 color;
		alignas( 16 ) DirectX::XMFLOAT3 ambient;
		float intensity;
		float padding[3];
		DirectX::XMMATRIX lightViewProjectionMatrix;
	};
	// TODO: Day/Night Cycles
	DirectX::XMFLOAT3 homePos{ 0.0f, 200.0f, 10.0f };
	float homePitch = PI / 2.0f;
	float homeYaw = -PI;
	DirectionalLightCBuf home;
	DirectionalLightCBuf cbData;
	mutable DeferredFullScreen mesh;
	mutable Bind::PixelConstantBuffer<DirectionalLightCBuf> cbuf;
	std::shared_ptr<Camera> pCamera;
};
