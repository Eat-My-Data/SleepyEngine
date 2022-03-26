#pragma once
#include "Drawable.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../Camera/Camera.h"
#include "../Drawable/SolidCone.h"
#include "../Drawable/DeferredSolidCone.h"

class SpotLight
{
public:
	SpotLight( Graphics& gfx, DirectX::XMFLOAT3 pospos = { 20.0f,15.0f,5.0f }, f32 scale = 1.0f );
	void SpawnControlWindow();
	void Reset() noexcept;
	void Submit( size_t channels );
	void Bind( Graphics& gfx, DirectX::FXMMATRIX view ) const noexcept;
	void LinkTechniques( Rgph::RenderGraph& );
	std::shared_ptr<Camera> ShareCamera() const noexcept;
	void ToggleRenderTechnique( Graphics& gfx, const std::string& renderTechnique );
	struct SpotLightCBuf
	{
		alignas( 16 ) DirectX::XMFLOAT3 pos;
		alignas( 16 ) DirectX::XMFLOAT3 color;
		alignas( 16 ) DirectX::XMFLOAT3 lightDirection;
		float range;
		float outerRadius;
		float innerRadius;
		float pitch;
		float yaw;
		float intensity;
		float padding[2];
		DirectX::XMMATRIX spotViewProjectionMatrix;
	};
	SpotLightCBuf home;
	SpotLightCBuf cbData;
	mutable SolidCone mesh;
	mutable DeferredSolidCone dMesh;
	mutable Bind::PixelConstantBuffer<SpotLightCBuf> cbuf;
	std::shared_ptr<Camera> pCamera;
	// TODO: Fix where the spot light lookAt is, maybe draw lookAt?
};
