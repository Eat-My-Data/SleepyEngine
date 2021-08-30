#include "SolidSphere.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Blender.h"
#include "../ResourceManager/Vertex.h"
#include "../ResourceManager/Geometry/Sphere.h"


SolidSphere::SolidSphere( GraphicsDeviceInterface& gdi, float radius )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Sphere::Make();
	model.Transform( dx::XMMatrixScaling( radius, radius, radius ) );
	const auto geometryTag = "$sphere." + std::to_string( radius );
	AddBind( VertexBuffer::Resolve( gdi, geometryTag, model.m_VBVertices ) );
	AddBind( IndexBuffer::Resolve( gdi, geometryTag, model.m_vecOfIndices ) );

	auto pvs = VertexShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/SolidVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( PixelShader::Resolve( gdi, "../SleepyEngine/Shaders/Bin/SolidPS.cso" ) );

	AddBind( InputLayout::Resolve( gdi, model.m_VBVertices.GetLayout(), pvsbc ) );

	AddBind( Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<TransformCbuf>( gdi, *this ) );

	AddBind( Blender::Resolve( gdi, false ) );

	AddBind( Rasterizer::Resolve( gdi, false ) );
}

void SolidSphere::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}