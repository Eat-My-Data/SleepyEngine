#include "DeferredFullScreen.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/Bindables/Blender.h"
#include "../Renderer/Model/Vertex.h"
#include "../Geometry/FullScreen2T.h"
#include "../Bindable/Bindables/ConstantBuffers.h"
#include "../Bindable/Bindables/DynamicConstant.h"
#include "../Bindable/Bindables/ConstantBuffersEx.h"
#include "../Renderer/Jobber/TechniqueProbe.h"
#include "../Renderer/Channels.h"

DeferredFullScreen::DeferredFullScreen( Graphics& gfx )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = FullScreen2T::Make();
	const auto geometryTag = "$deferred_full_screen";

	pVertices = VertexBuffer::Resolve( gfx, geometryTag, model.m_VBVertices );
	pIndices = IndexBuffer::Resolve( gfx, geometryTag, model.m_vecOfIndices );
	pTopology = Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	{
		Technique lightGeometry{ "deferredLighting", Chan::main, false };
		Step only( "deferredLighting" );

		auto pvs = VertexShader::Resolve( gfx, "./Shaders/Bin/DeferredDirectionalLight_VS.cso" );
		only.AddBindable( InputLayout::Resolve( gfx, model.m_VBVertices.GetLayout(), *pvs ) );
		only.AddBindable( std::move( pvs ) );

		only.AddBindable( PixelShader::Resolve( gfx, "./Shaders/Bin/DeferredDirectionalLight_PS.cso" ) );
		// might need to add constant buffer?
		only.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

		//only.AddBindable( Blender::Resolve( gfx, false ) );

		only.AddBindable( Rasterizer::Resolve( gfx, false ) );

		lightGeometry.AddStep( std::move( only ) );
		AddTechnique( std::move( lightGeometry ) );
	}
}

DirectX::XMMATRIX DeferredFullScreen::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixIdentity();
}