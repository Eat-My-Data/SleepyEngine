#include "SolidCone.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Blender.h"
#include "../ResourceManager/Vertex.h"
#include "../ResourceManager/Geometry/Cone.h"


SolidCone::SolidCone( GraphicsDeviceInterface& gdi, f32 scale )
{
	/*using namespace Bind;
	namespace dx = DirectX;

	auto model = Cone::Make();
	model.Transform( dx::XMMatrixScaling( scale, scale, scale ) );
	const auto geometryTag = "cone." + std::to_string( scale );
	AddBind( VertexBuffer::Resolve( gdi, geometryTag, model.m_VBVertices ) );
	AddBind( IndexBuffer::Resolve( gdi, geometryTag, model.m_vecOfIndices ) );

	auto pvs = VertexShader::Resolve( gdi, "./Shaders/Bin/SolidVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( PixelShader::Resolve( gdi, "./Shaders/Bin/SolidPS.cso" ) );

	AddBind( InputLayout::Resolve( gdi, model.m_VBVertices.GetLayout(), pvsbc ) );

	AddBind( Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<TransformCbuf>( gdi, *this ) );

	AddBind( Blender::Resolve( gdi, false ) );

	AddBind( Rasterizer::Resolve( gdi, false ) );*/
}

void SolidCone::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void SolidCone::Rotate( f32 pitch, f32 yaw ) noexcept
{
	m_fPitch = pitch;
	m_fYaw = yaw;
}

DirectX::XMMATRIX SolidCone::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( m_fPitch, m_fYaw, 0.0f ) *
		DirectX::XMMatrixTranslationFromVector( DirectX::XMLoadFloat3( &pos ) );
}