#include "DirectionalLight.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Sampler.h"
#include "../Bindable/Bindables/Blender.h"
#include <algorithm>
#include "../Libraries/imgui/imgui.h"

DirectionalLight::DirectionalLight( Graphics& gfx )
	:
	mesh( gfx ),
	cbuf( gfx, 5 )
{
	//using namespace Bind;
	//namespace dx = DirectX;

	//auto pvs = VertexShader::Resolve( gdi, "./Shaders/Bin/LightVS.cso" );
	//auto pvsbc = pvs->GetBytecode();
	//AddBind( std::move( pvs ) );
	//AddBind( PixelShader::Resolve( gdi, "./Shaders/Bin/LightPS.cso" ) );
	//AddBind( Sampler::Resolve( gdi ) );
	//AddBind( Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	//AddBind( Rasterizer::Resolve( gdi, true ) );

	//m_pForwardLightMatrices = VertexConstantBuffer<ForwardMatrices>::Resolve( gdi, matrixcbuf, 1u );
	//AddBind( m_pForwardLightMatrices );	
	pCamera = std::make_shared<Camera>( gfx, "Directinal Light", DirectX::XMFLOAT3{ 0.0f, 200.0f, 0.0f }, PI / 2.0f, -PI, true );

	home = {
		{ 0.0f, -1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.7f,0.7f,0.7f },
		0.0f,
		{ 0.0f, 0.0f }, // padding
		{ pCamera->GetMatrix() * pCamera->GetProjection() }
	};

	Reset();
}

void DirectionalLight::SpawnControlWindow() noexcept
{
	if ( ImGui::Begin( "Directional Light" ) )
	{
		bool dirtyPos = false;
		const auto d = [&dirtyPos]( bool dirty ) {dirtyPos = dirtyPos || dirty; };

		// Rotate Light/Pass time controls?
		ImGui::Text( "Light Direction" );
		d( ImGui::SliderFloat( "X", &cbData.lightDirection.x, -1.0f, 1.0f, "%.1f" ) );
		d( ImGui::SliderFloat( "Y", &cbData.lightDirection.y, -1.0f, 1.0f, "%.1f" ) );
		d( ImGui::SliderFloat( "Z", &cbData.lightDirection.z, -1.0f, 1.0f, "%.1f" ) );

		ImGui::SliderFloat( "Intensity", &cbData.intensity, 0.0f, 1.0f );

		ImGui::Text( "Color/Ambience" );
		ImGui::ColorEdit3( "Diffuse Color", &cbData.color.x );
		ImGui::ColorEdit3( "Ambience", &cbData.ambient.x );

		if ( ImGui::Button( "Reset" ) )
		{
			Reset();
		}
	}
	ImGui::End();
}

void DirectionalLight::Reset() noexcept
{
	cbData = home;
}

void DirectionalLight::Submit( size_t channels ) const noexcept( !IS_DEBUG )
{
	mesh.Submit( channels );
}

void DirectionalLight::Bind( Graphics& gfx, DirectX::FXMMATRIX view ) const noexcept
{
	cbuf.Update( gfx, cbData );
	cbuf.Bind( gfx );
}

void DirectionalLight::LinkTechniques( Rgph::RenderGraph& rg )
{
	mesh.LinkTechniques( rg );
}

std::shared_ptr<Camera> DirectionalLight::ShareCamera() const noexcept
{
	return pCamera;
}

void DirectionalLight::ToggleRenderTechnique( Graphics& gfx, const std::string& renderTechnique )
{
	mesh.ToggleRenderTechnique( gfx, renderTechnique );
}














//DirectX::XMMATRIX DirectionalLight::GetTransformXM() const noexcept
//{
//	return DirectX::XMMatrixTranslation( 1.0f, 1.0f, 1.0f );
//}
//
//void DirectionalLight::Update( Graphics& gdi )
//{
//	m_StructuredBufferData.lightViewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();
//
//	matrixcbuf.lightViewMatrix = GetViewMatrix();
//	matrixcbuf.lightProjMatrix = GetProjectionMatrix();
//
//	//m_pForwardLightMatrices->Update( gdi, matrixcbuf );
//	//m_pForwardLightMatrices->Bind( gdi );
//}
//
//void DirectionalLight::DrawControlPanel()
//{
//	ImGui::Text( "Directional Light" );
//	ImGui::ColorEdit3( "Color", &m_StructuredBufferData.color.x );
//	ImGui::SliderAngle( "Pitch", &m_fPitch, 0.995f * -90.0f, 0.995f * 90.0f );
//	ImGui::SliderAngle( "Yaw", &m_fYaw, -180.0f, 180.0f );
//}
//
//void DirectionalLight::Draw( Graphics& gdi ) const noexcept
//{
//	// bindables
//	//for ( auto& b : binds )
//	//{
//	//	b->Bind( gdi );
//	//}
//
//	gdi.GetContext()->Draw( 3, 0 );
//}
//
//void DirectionalLight::Translate( DirectX::XMFLOAT3 translation )
//{}
//
//void DirectionalLight::Rotate( const f32 dx, const f32 dy )
//{
//	m_fYaw = m_fYaw + dx * 0.05f;
//	m_fPitch = m_fPitch + dy * 0.05f;
//}
//
//DirectX::XMMATRIX DirectionalLight::GetViewMatrix() noexcept
//{
//	// completely changes on tiny rotation
//	using namespace DirectX;
//
//	const XMVECTOR forwardBaseVector = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
//	// apply the camera rotations to a base vector
//	const auto lookVector = XMVector3Transform( forwardBaseVector,
//		XMMatrixRotationRollPitchYaw( m_fPitch, m_fYaw, 0.0f )
//	);
//	// generate camera transform (applied to all objects to arrange them relative
//	// to camera position/orientation in world) from cam position and direction
//	// camera "top" always faces towards +Y (cannot do a barrel roll)
//	const auto camPosition = XMLoadFloat3( &homePos );
//	const auto camTarget = camPosition + lookVector;
//	return XMMatrixLookAtLH( camPosition, camTarget, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
//}
//
//DirectX::XMMATRIX DirectionalLight::GetProjectionMatrix() noexcept
//{
//	return DirectX::XMMatrixOrthographicLH( 400.0f, 400.0f, 1.0f, 1200.0f );
//}