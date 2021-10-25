#include "SpotLight.h"
#include "Cube.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Sampler.h"
#include "../Bindable/Bindables/Blender.h"
#include "../ResourceManager/Geometry/Cone.h"
#include <algorithm>
#include <d3dcompiler.h>
#include <math.h>

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
	//AddBind( PixelShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/SpotLightPS.cso" ) );
	ID3DBlob* pBlob;
	D3DReadFileToBlob( L"./Shaders/Bin/SpotLightPS.cso", &pBlob );
	gdi.GetDevice()->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader );
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

	// CameraIsInside Resources
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

DirectX::XMMATRIX SpotLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( m_fPitch - ( PI / 2.0f ), m_fYaw, 0.0f ) *
		DirectX::XMMatrixTranslation( m_StructuredBufferData.pos.x, m_StructuredBufferData.pos.y, m_StructuredBufferData.pos.z );
}

void SpotLight::Update( GraphicsDeviceInterface& gdi, DirectX::XMFLOAT3 camPos )
{
	m_StructuredBufferData.spotViewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();

	matrixcbuf.lightViewMatrix = GetViewMatrix();
	matrixcbuf.lightProjMatrix = GetProjectionMatrix();

	m_pForwardLightMatrices->Update( gdi, matrixcbuf );
	m_pForwardLightMatrices->Bind( gdi );
}

void SpotLight::Draw( GraphicsDeviceInterface& gdi )
{
	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gdi );
	}

	// get camera matrix from view matrix
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( gdi.GetViewMatrix() );
	DirectX::XMMATRIX cameraMatrix = DirectX::XMMatrixInverse( &determinant, gdi.GetViewMatrix() );

	DirectX::XMFLOAT3 camPos = {
		 cameraMatrix.r[3].m128_f32[0],
		 cameraMatrix.r[3].m128_f32[1],
		 cameraMatrix.r[3].m128_f32[2]
	};

	//// figure out if camera is inside spot light
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

	//gdi.DrawIndexed( pIndexBuffer->GetCount() );
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
	return DirectX::XMMatrixPerspectiveFovLH( PI / 2, 1.0f, 0.005f, m_StructuredBufferData.range );
}

bool SpotLight::CameraIsInside( DirectX::XMFLOAT3 camPos )
{
	// tried following: https://stackoverflow.com/questions/12826117/how-can-i-detect-if-a-point-is-inside-a-cone-or-not-in-3d-space	
	float pMinusX_X = camPos.x - m_StructuredBufferData.pos.x;
	float pMinusX_Y = camPos.y - m_StructuredBufferData.pos.y;
	float pMinusX_Z = camPos.z - m_StructuredBufferData.pos.z;

	float length = sqrt( sq( m_StructuredBufferData.lightDirection.x ) + sq( m_StructuredBufferData.lightDirection.y ) + sq( m_StructuredBufferData.lightDirection.z ) );
	DirectX::XMFLOAT3 normalizedLightDirection = { m_StructuredBufferData.lightDirection.x / length, m_StructuredBufferData.lightDirection.y / length, m_StructuredBufferData.lightDirection.z / length };
	float cone_dist = (pMinusX_X * normalizedLightDirection.x) + ( pMinusX_Y * normalizedLightDirection.y / length ) + ( pMinusX_Z * normalizedLightDirection.z );
	
	if ( 0.0f <= cone_dist && cone_dist <= 10.0f )
		return false;

	float cone_radius = ( cone_dist / 10.0f ) * m_StructuredBufferData.outerRadius;
	float PMinusXMinusConeDistX = pMinusX_X - cone_dist;
	float PMinusXMinusConeDistY = pMinusX_Y - cone_dist;
	float PMinusXMinusConeDistZ = pMinusX_Z - cone_dist;
	DirectX::XMFLOAT3 whatINeedToSquare = { normalizedLightDirection.x * PMinusXMinusConeDistX, normalizedLightDirection.y * PMinusXMinusConeDistY, normalizedLightDirection.z * PMinusXMinusConeDistZ };
	float orth_distance = sqrt( sq( whatINeedToSquare.x) + sq( whatINeedToSquare.y ) + sq( whatINeedToSquare.z ) );

	return orth_distance > cone_radius;
}
