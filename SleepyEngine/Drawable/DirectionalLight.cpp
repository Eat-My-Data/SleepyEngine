#include "DirectionalLight.h"
#include "Cube.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Sampler.h"

DirectionalLight::DirectionalLight( GraphicsDeviceInterface& gfx )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto pvs = VertexShader::Resolve( gfx, "../SleepyEngine/Shaders/Bin/LightVS.cso" );

	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( PixelShader::Resolve( gfx, "../SleepyEngine/Shaders/Bin/LightPS.cso" ) );\
	AddBind( Sampler::Resolve( gfx ) );

	pcs = PixelConstantBuffer<LightBufferType>::Resolve( gfx, lbuf, 0u );
	AddBind( pcs );

	pcs2 = PixelConstantBuffer<CamPosBuffer>::Resolve( gfx, cambuf, 1u );
	AddBind( pcs2 );

	AddBind( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	//AddBind( Blender::Resolve( gfx, true ) );
	AddBind( Rasterizer::Resolve( gfx, true ) );
}

void DirectionalLight::SetDirection( DirectX::XMFLOAT3 direction ) noexcept
{
	// don't need to change directional light's direction
}

void DirectionalLight::SpawnControlWindow( GraphicsDeviceInterface& gfx ) noexcept
{
}

DirectX::XMMATRIX DirectionalLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( 1.0f, 1.0f, 1.0f );
}

void DirectionalLight::UpdateCBuffers( GraphicsDeviceInterface& gdi, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix )
{
	// get camera matrix from view matrix
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( viewMatrix );
	DirectX::XMMATRIX cameraMatrix = DirectX::XMMatrixInverse( &determinant, viewMatrix );
	lbuf.cameraMatrix = cameraMatrix;

	// get inverse of the projection matrix
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( projectionMatrix );
	DirectX::XMMATRIX projInvMatrix = DirectX::XMMatrixInverse( &determinant2, projectionMatrix );
	lbuf.projInvMatrix = projInvMatrix;
	pcs->Update( gdi, lbuf );

	cambuf.camPos = DirectX::XMFLOAT3( cameraMatrix.r[3].m128_f32[0], cameraMatrix.r[3].m128_f32[1], cameraMatrix.r[3].m128_f32[2] );
	pcs2->Update( gdi, cambuf );
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