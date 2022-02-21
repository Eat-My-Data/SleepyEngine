//#pragma once
//#include "Drawable.h"
//#include "../Bindable/Bindables/ConstantBuffers.h"
//#include "../SceneManager/RenderTechnique.h"
//#include "../SceneManager/Camera.h"
//
//class DirectionalLight : public Drawable
//{
//public:
//	DirectionalLight( Graphics& gdi );
//	DirectX::XMMATRIX GetTransformXM() const noexcept override;
//	void Update( Graphics& gdi );
//	void DrawControlPanel();
//	void Draw( Graphics& gdi ) const noexcept;
//public:
//	void Translate( DirectX::XMFLOAT3 translation );
//	void Rotate( const f32 dx, const f32 dy );
//	DirectX::XMMATRIX GetViewMatrix() noexcept;
//	DirectX::XMMATRIX GetProjectionMatrix() noexcept;
//public:
//	f32 m_fPitch = PI / 2.0f;
//	f32 m_fYaw = -PI;
//public:
//	struct DirectionalLightData
//	{
//		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f };
//		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
//		float att = 0.7f;
//		float padding;
//		DirectX::XMMATRIX lightViewProjectionMatrix;
//	};
//	DirectionalLightData m_StructuredBufferData;
//	DirectX::XMFLOAT3 homePos = { 0.0f, 200.0f, 10.0f };
//private:
//	struct ForwardMatrices
//	{
//		DirectX::XMMATRIX lightViewMatrix;
//		DirectX::XMMATRIX lightProjMatrix;
//	} matrixcbuf;
//	std::shared_ptr<Bind::VertexConstantBuffer<ForwardMatrices>> m_pForwardLightMatrices;
//};
