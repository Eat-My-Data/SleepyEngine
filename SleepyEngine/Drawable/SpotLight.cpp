#include "SpotLight.h"
#include "Cube.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Sampler.h"
#include "../Bindable/Bindables/Blender.h"
#include <algorithm>

SpotLight::SpotLight( GraphicsDeviceInterface& gdi )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto pvs = VertexShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/LightVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );
	AddBind( PixelShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/LightPS.cso" ) );
	AddBind( Sampler::Resolve( gdi ) );
	AddBind( Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	AddBind( Rasterizer::Resolve( gdi, true ) );

	m_pForwardLightMatrices = VertexConstantBuffer<ForwardMatrices>::Resolve( gdi, matrixcbuf, 2u );
	AddBind( m_pForwardLightMatrices );	
}


DirectX::XMMATRIX SpotLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( 1.0f, 1.0f, 1.0f );
}

void SpotLight::Update( GraphicsDeviceInterface& gdi, DirectX::XMFLOAT3 camPos )
{
	// get camera matrix from view matrix
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( gdi.GetViewMatrix() );
	DirectX::XMMATRIX cameraMatrix = DirectX::XMMatrixInverse( &determinant, gdi.GetViewMatrix() );

	// get inverse of the projection matrix
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( gdi.GetProjMatrix() );
	DirectX::XMMATRIX projInvMatrix = DirectX::XMMatrixInverse( &determinant2, gdi.GetProjMatrix() );

	
	matrixcbuf.lightViewMatrix = m_PerspectiveCamera.GetViewMatrix();
	matrixcbuf.lightProjMatrix = m_PerspectiveCamera.GetProjectionMatrix();

	m_pForwardLightMatrices->Update( gdi, matrixcbuf );
	m_pForwardLightMatrices->Bind( gdi );
}

void SpotLight::Draw( GraphicsDeviceInterface& gdi ) const noexcept
{
	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gdi );
	}

	gdi.GetContext()->Draw( 3, 0 );
}

void SpotLight::Translate( DirectX::XMFLOAT3 translation )
{
	m_PerspectiveCamera.Translate( translation );
	m_StructuredBufferData.pos.x += translation.x;
	m_StructuredBufferData.pos.y += translation.y;
	m_StructuredBufferData.pos.z += translation.z;
}

void SpotLight::Rotate( const f32 dx, const f32 dy )
{
	m_PerspectiveCamera.Rotate( dx, dy );

	m_StructuredBufferData.lightDirection.x = m_PerspectiveCamera.GetViewMatrix().r[2].m128_f32[0];
	m_StructuredBufferData.lightDirection.y = m_PerspectiveCamera.GetViewMatrix().r[2].m128_f32[1];
	m_StructuredBufferData.lightDirection.z = m_PerspectiveCamera.GetViewMatrix().r[2].m128_f32[2];
}

DirectX::XMMATRIX SpotLight::GetViewMatrix() noexcept
{
	return m_PerspectiveCamera.GetViewMatrix();
}

DirectX::XMMATRIX SpotLight::GetProjectionMatrix() noexcept
{
	return m_PerspectiveCamera.GetProjectionMatrix();
}
