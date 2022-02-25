#pragma once
#include "RenderQueuePass.h"
#include "../Job.h"
#include <vector>
#include "../Sink.h"
#include "../Source.h"
#include "../../../Bindable/Bindables/Stencil.h"
#include "../../../Camera/Camera.h"
#include "../../../Bindable/Bindables/DepthStencil.h"
#include "../../../Bindable/Bindables/ShadowCameraCBuf.h"
#include "../../../Bindable/Bindables/Sampler.h"
#include "../../../Bindable/Bindables/Rasterizer.h"
#include "../../../Bindable/Bindables/DepthStencil.h"
#include "../../../Geometry/Cube.h"
#include "../../../Bindable/Bindables/CubeTexture.h"
#include "../../../Bindable/Bindables/SkyboxTransformCbuf.h"
#include "../../../Bindable/Bindables/VertexBuffer.h"
#include "../../../Bindable/Bindables/IndexBuffer.h"
#include "../../../Bindable/Bindables/Topology.h"
#include "../../../Bindable/Bindables/InputLayout.h"

class Graphics;

namespace Rgph
{
	class SkyboxPass : public BindingPass
	{
	public:
		SkyboxPass( Graphics& gfx, std::string name )
			:
			BindingPass( std::move( name ) )
		{
			using namespace Bind;
			RegisterSink( DirectBufferSink<RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSink( DirectBufferSink<DepthStencil>::Make( "depthStencil", depthStencil ) );
			AddBind( std::make_shared<CubeTexture>( gfx, "..\\Images\\SpaceBox" ) );
			AddBind( Stencil::Resolve( gfx, Stencil::Mode::DepthFirst ) );
			AddBind( Sampler::Resolve( gfx, Sampler::Type::Bilinear ) );
			AddBind( Rasterizer::Resolve( gfx, true ) );
			AddBind( std::make_shared<SkyboxTransformCbuf>( gfx ) );
			AddBind( PixelShader::Resolve( gfx, "./Shaders/Bin/Skybox_PS.cso" ) );
			AddBind( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
			{
				auto pvs = Bind::VertexShader::Resolve( gfx, "./Shaders/Bin/Skybox_VS.cso" );
				auto model = Cube::Make();
				model.Transform( DirectX::XMMatrixScaling( 3.0f, 3.0f, 3.0f ) );
				const auto geometryTag = "$cube_map";
				AddBind( VertexBuffer::Resolve( gfx, geometryTag, model.m_VBVertices ) );
				count = (UINT)model.m_vecOfIndices.size();
				AddBind( IndexBuffer::Resolve( gfx, geometryTag, model.m_vecOfIndices ) );
				AddBind( InputLayout::Resolve( gfx, model.m_VBVertices.GetLayout(), *pvs ) );
				AddBind( std::move( pvs ) );
			}
			RegisterSource( DirectBufferSource<RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSource( DirectBufferSource<DepthStencil>::Make( "depthStencil", depthStencil ) );
		}
		void BindMainCamera( const Camera& cam ) noexcept
		{
			pMainCamera = &cam;
		}
		void Execute( Graphics& gfx ) const noxnd override
		{
			assert( pMainCamera );
			pMainCamera->BindToGraphics( gfx );
			BindAll( gfx );
			gfx.DrawIndexed( count );
		}
	private:
		UINT count;
		const Camera* pMainCamera = nullptr;
	};
}