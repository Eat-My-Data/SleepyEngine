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

	pVertices = VertexBuffer::Resolve( gdi, geometryTag, model.m_VBVertices );
	pIndices = IndexBuffer::Resolve( gdi, geometryTag, model.m_vecOfIndices );
	pTopology = Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	{
		Technique solid;
		Step only( "lambertian" );

		auto pvs = VertexShader::Resolve( gdi, "./Shaders/Bin/Solid_VS.cso" );
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable( std::move( pvs ) );

		only.AddBindable( PixelShader::Resolve( gdi, "./Shaders/Bin/Solid_PS.cso" ) );

		struct PSColorConstant
		{
			dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		only.AddBindable( PixelConstantBuffer<PSColorConstant>::Resolve( gdi, colorConst, 8u ) );

		only.AddBindable( InputLayout::Resolve( gdi, model.m_VBVertices.GetLayout(), pvsbc ) );

		only.AddBindable( std::make_shared<TransformCbuf>( gdi ) );

		only.AddBindable( Blender::Resolve( gdi, false ) );

		only.AddBindable( Rasterizer::Resolve( gdi, false ) );

		solid.AddStep( std::move( only ) );
		AddTechnique( std::move( solid ) );
	}*/
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