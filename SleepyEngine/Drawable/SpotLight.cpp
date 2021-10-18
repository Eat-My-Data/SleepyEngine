#include "SpotLight.h"
#include "Cube.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Sampler.h"
#include "../Bindable/Bindables/Blender.h"
#include "../ResourceManager/Geometry/Cone.h"
#include <algorithm>

SpotLight::SpotLight( GraphicsDeviceInterface& gdi, f32 scale )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cone::Make();
	model.Transform( dx::XMMatrixScaling( scale, scale, scale ) );
	model.Transform( dx::XMMatrixTranslation( 0.0f, -scale + 1.0f, 0.0f ) );
	const auto geometryTag = "cone2." + std::to_string( scale );
	AddBind( VertexBuffer::Resolve( gdi, geometryTag, model.m_VBVertices ) );
	AddBind( IndexBuffer::Resolve( gdi, geometryTag, model.m_vecOfIndices ) );
	auto pvs = VertexShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/SpotLightVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );
	AddBind( PixelShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/SpotLightPS.cso" ) );
	AddBind( Sampler::Resolve( gdi ) );
	AddBind( Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	AddBind( Rasterizer::Resolve( gdi, true ) );
	AddBind( std::make_shared<TransformCbuf>( gdi, *this ) );
	m_pForwardLightMatrices = VertexConstantBuffer<ForwardMatrices>::Resolve( gdi, matrixcbuf, 2u );
	AddBind( m_pForwardLightMatrices );	

	m_pSolidCone = new SolidCone( gdi, 1.0f );
	m_pSolidCone->SetPos( m_StructuredBufferData.pos );
	m_pSolidCone->Rotate( m_fPitch - ( PI / 2.0f), m_fYaw );
	m_StructuredBufferData.lightDirection.x = GetViewMatrix().r[2].m128_f32[0];
	m_StructuredBufferData.lightDirection.y = GetViewMatrix().r[2].m128_f32[1];
	m_StructuredBufferData.lightDirection.z = GetViewMatrix().r[2].m128_f32[2];
}

DirectX::XMMATRIX SpotLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( m_fPitch - ( PI / 2.0f ), m_fYaw, 0.0f ) *
		DirectX::XMMatrixTranslation( m_StructuredBufferData.pos.x, m_StructuredBufferData.pos.y, m_StructuredBufferData.pos.z );
}

void SpotLight::Update( GraphicsDeviceInterface& gdi, DirectX::XMFLOAT3 camPos )
{
	// get camera matrix from view matrix
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( GetViewMatrix() );
	DirectX::XMMATRIX cameraMatrix = DirectX::XMMatrixInverse( &determinant, GetViewMatrix() );

	// get inverse of the projection matrix
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( GetProjectionMatrix() );
	DirectX::XMMATRIX projInvMatrix = DirectX::XMMatrixInverse( &determinant2, GetProjectionMatrix() );

	m_StructuredBufferData.spotViewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();
	matrixcbuf.lightViewMatrix = GetViewMatrix();
	matrixcbuf.lightProjMatrix = GetProjectionMatrix();

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

	gdi.DrawIndexed( pIndexBuffer->GetCount() );
}

void SpotLight::Translate( DirectX::XMFLOAT3 translation )
{
	m_StructuredBufferData.pos.x += translation.x;
	m_StructuredBufferData.pos.y += translation.y;
	m_StructuredBufferData.pos.z += translation.z;
	m_pSolidCone->SetPos( m_StructuredBufferData.pos );
}

void SpotLight::Rotate( const f32 dx, const f32 dy )
{
	// Fix rotation to not roll camera
	m_fYaw = wrap_angle( m_fYaw + dx * 0.004f );
	m_fPitch = std::clamp( m_fPitch + dy * 0.004f, 0.995f * -PI, 0.995f * PI );
	m_pSolidCone->Rotate( m_fPitch - ( PI / 2.0f ), m_fYaw );
	DirectX::XMMATRIX tView = DirectX::XMMatrixTranspose( GetViewMatrix() );
	m_StructuredBufferData.lightDirection.x = tView.r[2].m128_f32[0];
	m_StructuredBufferData.lightDirection.y = tView.r[2].m128_f32[1];
	m_StructuredBufferData.lightDirection.z = tView.r[2].m128_f32[2];
}

DirectX::XMMATRIX SpotLight::GetViewMatrix() noexcept
{
	using namespace DirectX;

	const XMVECTOR forwardBaseVector = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform( forwardBaseVector,
		XMMatrixRotationRollPitchYaw( m_fPitch, m_fYaw, 0.0f )
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3( &m_StructuredBufferData.pos );
	const auto camTarget = camPosition + lookVector;
	return XMMatrixLookAtLH( camPosition, camTarget, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
}

DirectX::XMMATRIX SpotLight::GetProjectionMatrix() noexcept
{
	return DirectX::XMMatrixPerspectiveFovLH( PI / 2, 1.0f, 0.05f, 20.0f );
}