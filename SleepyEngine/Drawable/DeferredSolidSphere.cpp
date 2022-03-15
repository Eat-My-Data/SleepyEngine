#include "DeferredSolidSphere.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Blender.h"
#include "../Renderer/Model/Vertex.h"
#include "../Geometry/Sphere.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../Bindable/Bindables/DynamicConstant.h"
#include "../Bindable/Bindables/ConstantBuffersEx.h"
#include "../Renderer/Jobber/TechniqueProbe.h"
#include "../Renderer/Channels.h"

DeferredSolidSphere::DeferredSolidSphere( Graphics& gdi, float radius )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Sphere::Make();
	model.Transform( dx::XMMatrixScaling( radius, radius, radius ) );
	const auto geometryTag = "$deferred_sphere." + std::to_string( radius );

	pVertices = VertexBuffer::Resolve( gdi, geometryTag, model.m_VBVertices );
	pIndices = IndexBuffer::Resolve( gdi, geometryTag, model.m_vecOfIndices );
	pTopology = Topology::Resolve( gdi, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	{
		Technique lightGeometry{ "deferredLighting", Chan::main, false };
		Step only( "deferredLighting" );

		auto pvs = VertexShader::Resolve( gdi, "./Shaders/Bin/DeferredPointLight_VS.cso" );
		only.AddBindable( InputLayout::Resolve( gdi, model.m_VBVertices.GetLayout(), *pvs ) );
		only.AddBindable( std::move( pvs ) );

		only.AddBindable( PixelShader::Resolve( gdi, "./Shaders/Bin/DeferredPointLight_PS.cso" ) );
		// might need to add constant buffer?
		only.AddBindable( std::make_shared<TransformCbuf>( gdi ) );

		//only.AddBindable( Blender::Resolve( gdi, false ) );

		only.AddBindable( Rasterizer::Resolve( gdi, false ) );

		lightGeometry.AddStep( std::move( only ) );
		AddTechnique( std::move( lightGeometry ) );
	}
}

void DeferredSolidSphere::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX DeferredSolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}