
#include "PointLight.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Sampler.h"
#include "../ResourceManager/Vertex.h"
#include "../ResourceManager/Geometry/Sphere.h"
#include <d3dcompiler.h>

PointLight::PointLight( GraphicsDeviceInterface& gdi, float radius )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Sphere::Make();
	model.Transform( dx::XMMatrixScaling( radius, radius, radius ) );
	const auto geometryTag = "$sphere." + std::to_string( radius );
	AddBind( VertexBuffer::Resolve( gdi, geometryTag, model.m_VBVertices ) );
	AddBind( IndexBuffer::Resolve( gdi, geometryTag, model.m_vecOfIndices ) );

	auto pvs = VertexShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/PointLightVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	ID3DBlob* pBlob;
	D3DReadFileToBlob( L"../SleepyEngine/Shaders/Bin/PointLightPS.cso", &pBlob );
	gdi.GetDevice()->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader );
	AddBind( PixelShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/PointLightPS.cso" ) );
	AddBind( Sampler::Resolve( gdi ) );

	Dvtx::VertexBuffer vbuf( std::move(
		Dvtx::VertexLayout{}
		.Append( Dvtx::VertexLayout::Position3D )
	) );
	AddBind( InputLayout::Resolve( gdi, vbuf.GetLayout(), pvsbc ) );

	AddBind( Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<TransformCbuf>( gdi, *this ) );


	D3D11_DEPTH_STENCIL_DESC dsDesInsideLight = {};
	dsDesInsideLight.DepthEnable = TRUE;
	dsDesInsideLight.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesInsideLight.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	HRESULT hr = gdi.GetDevice()->CreateDepthStencilState( &dsDesInsideLight, &pDSStateInsideLighting );
	if ( FAILED( hr ) )
	{
		throw std::exception();
	}

	D3D11_DEPTH_STENCIL_DESC dsDescInfrontBackFace = {};
	dsDescInfrontBackFace.DepthEnable = TRUE;
	dsDescInfrontBackFace.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDescInfrontBackFace.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	dsDescInfrontBackFace.StencilEnable = TRUE;
	dsDescInfrontBackFace.StencilReadMask = 0xFF;
	dsDescInfrontBackFace.StencilWriteMask = 0xFF;
	dsDescInfrontBackFace.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDescInfrontBackFace.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDescInfrontBackFace.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER;
	dsDescInfrontBackFace.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDescInfrontBackFace.BackFace = dsDescInfrontBackFace.FrontFace;
	hr = gdi.GetDevice()->CreateDepthStencilState( &dsDescInfrontBackFace, &pDSStateInfrontBackFaceOfLight );
	if ( FAILED( hr ) )
	{
		throw std::exception();
	}

	D3D11_DEPTH_STENCIL_DESC dsDescBehindFrontFace = {};
	dsDescBehindFrontFace.DepthEnable = TRUE;
	dsDescBehindFrontFace.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDescBehindFrontFace.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDescBehindFrontFace.StencilEnable = TRUE;
	dsDescBehindFrontFace.StencilReadMask = 0xFF;
	dsDescBehindFrontFace.StencilWriteMask = 0xFF;
	dsDescBehindFrontFace.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDescBehindFrontFace.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	dsDescBehindFrontFace.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsDescBehindFrontFace.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	dsDescBehindFrontFace.BackFace = dsDescInfrontBackFace.FrontFace;
	hr = gdi.GetDevice()->CreateDepthStencilState( &dsDescBehindFrontFace, &pDSStateLightingBehindFrontFaceOfLight );
	if ( FAILED( hr ) )
	{
		throw std::exception();
	}

	// Setup rasterizer state inside
	D3D11_RASTERIZER_DESC rasterizerDescInside;
	ZeroMemory( &rasterizerDescInside, sizeof( rasterizerDescInside ) );
	rasterizerDescInside.CullMode = D3D11_CULL_FRONT;
	rasterizerDescInside.FillMode = D3D11_FILL_SOLID;
	rasterizerDescInside.DepthClipEnable = false;

	gdi.GetDevice()->CreateRasterizerState( &rasterizerDescInside, &rasterizerInside );

	// Setup rasterizer state outside
	D3D11_RASTERIZER_DESC rasterizerDescOutside;
	ZeroMemory( &rasterizerDescOutside, sizeof( rasterizerDescOutside ) );
	rasterizerDescOutside.CullMode = D3D11_CULL_BACK;
	rasterizerDescOutside.FillMode = D3D11_FILL_SOLID;
	rasterizerDescOutside.DepthClipEnable = false;

	gdi.GetDevice()->CreateRasterizerState( &rasterizerDescOutside, &rasterizerOutside );
	
}

DirectX::XMMATRIX PointLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( m_StructuredBufferData.pos.x, m_StructuredBufferData.pos.y, m_StructuredBufferData.pos.z );
}

void PointLight::Update( DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, DirectX::XMFLOAT3 camPos )
{
		DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( viewMatrix );
		DirectX::XMMATRIX cameraMatrix = DirectX::XMMatrixInverse( &determinant, viewMatrix );
		m_StructuredBufferData.cameraMatrix = cameraMatrix;
		// get inverse of the projection matrix
		DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( projectionMatrix );
		DirectX::XMMATRIX projInvMatrix = DirectX::XMMatrixInverse( &determinant2, projectionMatrix );
		m_StructuredBufferData.projInvMatrix = projInvMatrix;
		// update camera position
		m_StructuredBufferData.camPos = camPos;
}

void PointLight::Draw( GraphicsDeviceInterface& gdi )
{
	//m_SolidSphere->Draw( gdi );

	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gdi );
	}

	// figure out if camera is inside point light
	if ( CameraIsInside( m_StructuredBufferData.camPos ) )
	{
		gdi.GetContext()->PSSetShader( pPixelShader, nullptr, 0u );
		gdi.GetContext()->RSSetState( rasterizerInside );
		gdi.GetContext()->OMSetDepthStencilState( pDSStateInsideLighting, 1u );

		// draw
		gdi.DrawIndexed( pIndexBuffer->GetCount() );
	}
	else
	{
		gdi.GetContext()->PSSetShader( nullptr, nullptr, 0u );
		gdi.GetContext()->RSSetState( rasterizerInside );
		gdi.GetContext()->OMSetDepthStencilState( pDSStateInfrontBackFaceOfLight, 0x10 );

		// draw
		gdi.DrawIndexed( pIndexBuffer->GetCount() );

		gdi.GetContext()->PSSetShader( pPixelShader, nullptr, 0u );
		gdi.GetContext()->RSSetState( rasterizerOutside );
		gdi.GetContext()->OMSetDepthStencilState( pDSStateLightingBehindFrontFaceOfLight, 0x10 );

		// draw
		gdi.DrawIndexed( pIndexBuffer->GetCount() );
	}
}

void PointLight::Translate( DirectX::XMFLOAT3 vec )
{
	m_StructuredBufferData.pos.x += vec.x;
	m_StructuredBufferData.pos.y += vec.y;
	m_StructuredBufferData.pos.z += vec.z;
}

bool PointLight::CameraIsInside( DirectX::XMFLOAT3 camPos )
{
	float distFromCenterX = m_StructuredBufferData.pos.x - camPos.x;
	float distFromCenterY = m_StructuredBufferData.camPos.y - camPos.y;
	float distFromCenterZ = m_StructuredBufferData.camPos.z - camPos.z;
	float xSq = distFromCenterX * distFromCenterX;
	float ySq = distFromCenterY * distFromCenterY;
	float zSq = distFromCenterZ * distFromCenterZ;
	float distSq = xSq + ySq + zSq;

	float radiusSq = ( m_StructuredBufferData.radius + 0.5f ) * ( m_StructuredBufferData.radius + 0.5f );

	return distSq <= radiusSq;
}
