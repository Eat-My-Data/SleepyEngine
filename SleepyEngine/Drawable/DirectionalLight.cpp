#include "DirectionalLight.h"
#include "Cube.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Sampler.h"
#include "../Bindable/Bindables/Blender.h"
#include <algorithm>
#include "../Libraries/imgui/backends/imgui_impl_dx11.h"
#include "../Libraries/imgui/backends/imgui_impl_win32.h"

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

	matrixcbuf.lightViewMatrix = GetViewMatrix();
	matrixcbuf.lightProjMatrix = GetProjectionMatrix();

	m_pForwardLightMatrices->Update( gdi, matrixcbuf );
	m_pForwardLightMatrices->Bind( gdi );
}

void DirectionalLight::DrawControlPanel()
{
	ImGui::Text( "Directional Light" );
	ImGui::ColorEdit3( "Color", &m_StructuredBufferData.color.x );
	ImGui::SliderAngle( "Pitch", &m_fPitch, 0.995f * -90.0f, 0.995f * 90.0f );
	ImGui::SliderAngle( "Yaw", &m_fYaw, -180.0f, 180.0f );
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
	m_fYaw = m_fYaw + dx * 0.05f;
	m_fPitch = m_fPitch + dy * 0.05f;
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