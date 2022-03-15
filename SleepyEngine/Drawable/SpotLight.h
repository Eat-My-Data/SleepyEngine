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
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT3 lightDirection;
		float range;
		float outerRadius;
		float innerRadius;
		float pitch;
		float yaw;
		float padding[2];
		DirectX::XMMATRIX spotViewProjectionMatrix;
	};
	SpotLightCBuf home;
	SpotLightCBuf cbData;
	mutable SolidCone mesh;
	mutable DeferredSolidCone dMesh;
	mutable Bind::PixelConstantBuffer<SpotLightCBuf> cbuf;
	std::shared_ptr<Camera> pCamera;
	// TODO: Allow multiple shadow cameras for BindCamera

//public:
//	void Translate( DirectX::XMFLOAT3 translation );
//	void Rotate( const f32 dx, const f32 dy );
//	DirectX::XMMATRIX GetViewMatrix() noexcept;
//	DirectX::XMMATRIX GetProjectionMatrix() noexcept;
//	bool CameraIsInside( DirectX::XMFLOAT3 camPos );
//private:
//	struct ForwardMatrices
//	{
//		DirectX::XMMATRIX lightViewMatrix;
//		DirectX::XMMATRIX lightProjMatrix;
//	} matrixcbuf;
//	std::shared_ptr<Bind::VertexConstantBuffer<ForwardMatrices>> m_pForwardLightMatrices;
//private:
//	ID3D11PixelShader* pPixelShader;
//	ID3D11RasterizerState* rasterizerInside;
//	ID3D11RasterizerState* rasterizerOutside;
//	ID3D11DepthStencilState* pDSStateInfrontBackFaceOfLight;
//	ID3D11DepthStencilState* pDSStateLightingBehindFrontFaceOfLight;
//	ID3D11DepthStencilState* pDSStateInsideLighting;
	// TODO:
	// - draw look at vector for debugging
};
