#include "DirectionalLight.h"
#include "Cube.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Sampler.h"
#include "../Bindable/Bindables/Blender.h"

DirectionalLight::DirectionalLight( GraphicsDeviceInterface& gdi, RenderTechnique renderTechnique )
{
	using namespace Bind;
	namespace dx = DirectX;

	m_pDefferedLightPCbuf = PixelConstantBuffer<DeferredLightBuffer>::Resolve( gdi, lbuf, 0u );
	AddBind( m_pDefferedLightPCbuf );
	m_pCameraPCBuf = PixelConstantBuffer<CamBuffer>::Resolve( gdi, cambuf, 1u );
	AddBind( m_pCameraPCBuf );

	if ( renderTechnique == RenderTechnique::Deferred )
	{
		auto pvs = VertexShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/LightVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddBind( std::move( pvs ) );
		AddBind( PixelShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/LightPS.cso" ) );
		AddBind( Sampler::Resolve( gdi ) );
		AddBind( Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
		AddBind( Rasterizer::Resolve( gdi, true ) );
	}
	else if ( renderTechnique == RenderTechnique::Forward )
	{
		m_pForwardLightPCbuf = PixelConstantBuffer<ForwardLightBuffer>::Resolve( gdi, dlcbuf, 2u );
		AddBind( m_pForwardLightPCbuf );
		m_pForwardLightMatrices = VertexConstantBuffer<ForwaredMatrices>::Resolve( gdi, matrixcbuf, 1u );
		AddBind( m_pForwardLightMatrices );
	}
}


DirectX::XMMATRIX DirectionalLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( 1.0f, 1.0f, 1.0f );
}

void DirectionalLight::UpdateCBuffers( GraphicsDeviceInterface& gdi, DirectX::XMFLOAT3 camPos )
{
	// get camera matrix from view matrix
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( gdi.GetViewMatrix() );
	DirectX::XMMATRIX cameraMatrix = DirectX::XMMatrixInverse( &determinant, gdi.GetViewMatrix() );
	lbuf.cameraMatrix = cameraMatrix;

	// get inverse of the projection matrix
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( gdi.GetProjMatrix() );
	DirectX::XMMATRIX projInvMatrix = DirectX::XMMatrixInverse( &determinant2, gdi.GetProjMatrix() );
	lbuf.projInvMatrix = projInvMatrix;
	m_pDefferedLightPCbuf->Update( gdi, lbuf );

	cambuf.lightViewMatrix = m_OrthoCamera.GetViewMatrix();
	cambuf.lightProjMatrix = m_OrthoCamera.GetProjectionMatrix();
	cambuf.camPos = camPos;
	m_pCameraPCBuf->Update( gdi, cambuf );
}

void DirectionalLight::Draw( GraphicsDeviceInterface& gdi ) const noexcept
{
	// set blend state to default for geometry
	gdi.GetContext()->RSSetState( NULL );
	const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	gdi.GetContext()->OMSetBlendState( NULL, blendFactor, 0xFFFFFFFF );

	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gdi );
	}

	gdi.GetContext()->Draw( 3, 0 );
}

void DirectionalLight::Translate( DirectX::XMFLOAT3 translation )
{
	m_OrthoCamera.Translate( translation );
}

void DirectionalLight::Rotate( const f32 dx, const f32 dy )
{
	m_OrthoCamera.Rotate( dx, dy );
}

DirectX::XMMATRIX DirectionalLight::GetViewMatrix() noexcept
{
	return m_OrthoCamera.GetViewMatrix();
}

DirectX::XMMATRIX DirectionalLight::GetProjectionMatrix() noexcept
{
	return m_OrthoCamera.GetProjectionMatrix();
}

void DirectionalLight::UpdateForwardCBuffer( GraphicsDeviceInterface& gdi )
{
	matrixcbuf.lightViewMatrix = m_OrthoCamera.GetViewMatrix();
	matrixcbuf.lightProjMatrix = m_OrthoCamera.GetProjectionMatrix();
	m_pForwardLightMatrices->Update( gdi, matrixcbuf );
	m_pForwardLightMatrices->Bind( gdi );

	m_pForwardLightPCbuf->Update( gdi, dlcbuf );
	m_pForwardLightPCbuf->Bind( gdi );
}
