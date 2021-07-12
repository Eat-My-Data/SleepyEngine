
#include "PointLight.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Sampler.h"
#include "../ResourceManager/Vertex.h"
#include "../ResourceManager/Geometry/Sphere.h"
#include <d3dcompiler.h>

PointLight::PointLight( GraphicsDeviceInterface& gdi, float radius, RenderTechnique renderTechnique )
	: 
	m_RenderTechnique( renderTechnique ),
	m_pFCbuf( gdi )
{
	if ( renderTechnique == RenderTechnique::Deferred )
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

		colorConst.radius = radius;
		pcs = PixelConstantBuffer<PSColorConstant>::Resolve( gdi, colorConst, 0u );
		AddBind( pcs );

		pcs2 = PixelConstantBuffer<PSPositionConstant>::Resolve( gdi, posConst, 1u );
		AddBind( pcs2 );

		pcs3 = PixelConstantBuffer<CamPosBuffer>::Resolve( gdi, cambuf, 2u );
		AddBind( pcs3 );

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

		//=========================RASTERIZER=========================
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
		//=========================RASTERIZER=========================
	}
}

DirectX::XMMATRIX PointLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( posConst.lightPosition.x, posConst.lightPosition.y, posConst.lightPosition.z );
}

void PointLight::UpdateCBuffers( GraphicsDeviceInterface& gdi, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, DirectX::XMFLOAT3 camPos )
{
		DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( viewMatrix );
		DirectX::XMMATRIX cameraMatrix = DirectX::XMMatrixInverse( &determinant, viewMatrix );
		colorConst.cameraMatrix = cameraMatrix;

		// get inverse of the projection matrix
		DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( projectionMatrix );
		DirectX::XMMATRIX projInvMatrix = DirectX::XMMatrixInverse( &determinant2, projectionMatrix );
		colorConst.projInvMatrix = projInvMatrix;
		pcs->Update( gdi, colorConst );

		// update light position
		pcs2->Update( gdi, posConst );

		// update camera position
		cambuf.camPos = camPos;
		pcs3->Update( gdi, cambuf );
}

void PointLight::Draw( GraphicsDeviceInterface& gdi, DirectX::XMFLOAT3 camPos )
{
	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gdi );
	}

	// figure out if camera is inside point light
	if ( CameraIsInside( camPos ) )
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

	// clear shader resources
	ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr };
	gdi.GetContext()->PSSetShaderResources( 0, 4, null );
}

void PointLight::Translate( DirectX::XMFLOAT3 vec )
{
	if ( m_RenderTechnique == RenderTechnique::Forward )
	{
		m_ForwardCBufData.pos.x += vec.x;
		m_ForwardCBufData.pos.y += vec.y;
		m_ForwardCBufData.pos.z += vec.z;
	}
	else
	{
		posConst.lightPosition.x += vec.x;
		posConst.lightPosition.y += vec.y;
		posConst.lightPosition.z += vec.z;
	}
}

bool PointLight::CameraIsInside( DirectX::XMFLOAT3 camPos )
{
	float distFromCenterX = posConst.lightPosition.x - camPos.x;
	float distFromCenterY = posConst.lightPosition.y - camPos.y;
	float distFromCenterZ = posConst.lightPosition.z - camPos.z;
	float xSq = distFromCenterX * distFromCenterX;
	float ySq = distFromCenterY * distFromCenterY;
	float zSq = distFromCenterZ * distFromCenterZ;
	float distSq = xSq + ySq + zSq;

	float radiusSq = (colorConst.radius + 0.5f) * (colorConst.radius + 0.5f);

	return distSq <= radiusSq;
}

void PointLight::UpdateForwardCBuffer( GraphicsDeviceInterface& gdi )
{
	m_pFCbuf.Update( gdi, m_ForwardCBufData );
	m_pFCbuf.Bind( gdi );
}


