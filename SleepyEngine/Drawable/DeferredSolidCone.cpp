#include "DeferredSolidCone.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Blender.h"
#include "../Renderer/Model/Vertex.h"
#include "../Geometry/Cone.h"
#include "../Renderer/Channels.h"

DeferredSolidCone::DeferredSolidCone( Graphics& gdi, f32 scale )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cone::Make();
	model.Transform( dx::XMMatrixScaling( scale, scale, scale ) );
	const auto geometryTag = "deferred_cone." + std::to_string( scale );

	pVertices = VertexBuffer::Resolve( gdi, geometryTag, model.m_VBVertices );
	pIndices = IndexBuffer::Resolve( gdi, geometryTag, model.m_vecOfIndices );
	pTopology = Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	{
		Technique lightGeometry{ "deferredLighting", Chan::main, false };
		Step only( "deferredLighting" );

		auto pvs = VertexShader::Resolve( gdi, "./Shaders/Bin/DeferredSpotLight_VS.cso" );
		only.AddBindable( InputLayout::Resolve( gdi, model.m_VBVertices.GetLayout(), *pvs ) );
		only.AddBindable( std::move( pvs ) );

		only.AddBindable( PixelShader::Resolve( gdi, "./Shaders/Bin/DeferredSpotLight_PS.cso" ) );

		only.AddBindable( std::make_shared<TransformCbuf>( gdi ) );

		//only.AddBindable( Blender::Resolve( gdi, false ) );

		only.AddBindable( Rasterizer::Resolve( gdi, false ) );

		lightGeometry.AddStep( std::move( only ) );
		AddTechnique( std::move( lightGeometry ) );
	}
}

void DeferredSolidCone::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void DeferredSolidCone::Rotate( f32 pitch, f32 yaw ) noexcept
{
	m_fPitch = pitch;
	m_fYaw = yaw;
}

DirectX::XMMATRIX DeferredSolidCone::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( m_fPitch, m_fYaw, 0.0f ) *
		DirectX::XMMatrixTranslationFromVector( DirectX::XMLoadFloat3( &pos ) );
}