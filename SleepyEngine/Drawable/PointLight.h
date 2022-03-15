#pragma once
#include "../Graphics/Graphics.h"
#include "../Drawable/SolidSphere.h"
#include "../Drawable/DeferredSolidSphere.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../Macros/ConditionalNoexcept.h"

namespace Rgph
{
	class RenderGraph;
}

class Camera;

class PointLight
{
public:
	PointLight( Graphics& gfx, DirectX::XMFLOAT3 pos = { 10.0f,9.0f,2.5f }, float radius = 0.5f );
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Submit( size_t channels ) const noxnd;
	void Bind( Graphics& gfx, DirectX::FXMMATRIX view ) const noexcept;
	void LinkTechniques( Rgph::RenderGraph& );
	std::shared_ptr<Camera> ShareCamera() const noexcept;
	void ToggleRenderTechnique( Graphics& gfx, const std::string& renderTechnique );
private:
	struct PointLightCBuf
	{
		alignas( 16 ) DirectX::XMFLOAT3 pos;
		alignas( 16 ) DirectX::XMFLOAT3 ambient;
		alignas( 16 ) DirectX::XMFLOAT3 diffuseColor;
		alignas( 16 ) DirectX::XMFLOAT3 radius;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
private:
	PointLightCBuf home;
	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	mutable DeferredSolidSphere dMesh;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> cbuf;
	std::shared_ptr<Camera> pCamera;
};