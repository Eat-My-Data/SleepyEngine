#include "DirectionalLight.h"
#include "Cube.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Sampler.h"
#include "../Bindable/Bindables/Blender.h"
#include <algorithm>

DirectionalLight::DirectionalLight( GraphicsDeviceInterface& gdi )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto pvs = VertexShader::Resolve( gdi, "./Shaders/Bin/LightVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );
	AddBind( PixelShader::Resolve( gdi, "./Shaders/Bin/LightPS.cso" ) );
	AddBind( Sampler::Resolve( gdi ) );
	AddBind( Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	AddBind( Rasterizer::Resolve( gdi, true ) );

	m_pForwardLightMatrices = VertexConstantBuffer<ForwardMatrices>::Resolve( gdi, matrixcbuf, 1u );
	AddBind( m_pForwardLightMatrices );	
}


DirectX::XMMATRIX DirectionalLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( 1.0f, 1.0f, 1.0f );
}

void DirectionalLight::Update( GraphicsDeviceInterface& gdi, DirectX::XMFLOAT3 camPos )
{
	m_StructuredBufferData.lightViewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();

	DirectX::XMMATRIX tView = DirectX::XMMatrixTranspose( GetViewMatrix() );
	m_StructuredBufferData.lightDirection.x = tView.r[2].m128_f32[0];
	m_StructuredBufferData.lightDirection.y = tView.r[2].m128_f32[1];
	m_StructuredBufferData.lightDirection.z = tView.r[2].m128_f32[2];

	matrixcbuf.lightViewMatrix = GetViewMatrix();
	matrixcbuf.lightProjMatrix = GetProjectionMatrix();

	m_pForwardLightMatrices->Update( gdi, matrixcbuf );
	m_pForwardLightMatrices->Bind( gdi );
}

void DirectionalLight::Draw( GraphicsDeviceInterface& gdi ) const noexcept
{
	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gdi );
	}

	gdi.GetContext()->Draw( 3, 0 );
}

void DirectionalLight::Translate( DirectX::XMFLOAT3 translation )
{}

void DirectionalLight::Rotate( const f32 dx, const f32 dy )
{
	m_fYaw = wrap_angle( m_fYaw + dx );
	m_fPitch = std::clamp( m_fPitch + dy, 0.995f * -PI, 0.995f * PI );
}

DirectX::XMMATRIX DirectionalLight::GetViewMatrix() noexcept
{
	// completely changes on tiny rotation
	using namespace DirectX;

	const XMVECTOR forwardBaseVector = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform( forwardBaseVector,
		XMMatrixRotationRollPitchYaw( m_fPitch, m_fYaw, 0.0f )
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3( &homePos );
	const auto camTarget = camPosition + lookVector;
	return XMMatrixLookAtLH( camPosition, camTarget, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
}

DirectX::XMMATRIX DirectionalLight::GetProjectionMatrix() noexcept
{
	return DirectX::XMMatrixOrthographicLH( 400.0f, 400.0f, 1.0f, 1200.0f );
}

Camera& DirectionalLight::GetCamera()
{
	return m_OrthoCamera;
}
