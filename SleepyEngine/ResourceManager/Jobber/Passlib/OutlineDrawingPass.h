#pragma once
#include "RenderQueuePass.h"
#include "../Job.h"
#include <vector>
#include "../../../Bindable/Bindables/PixelShader.h"
#include "../../../Bindable/Bindables/VertexShader.h"
#include "../../../Bindable/Bindables/Stencil.h"
#include "../../../Bindable/Bindables/Rasterizer.h"

class GraphicsDeviceInterface;

namespace Rgph
{
	class OutlineDrawingPass : public RenderQueuePass
	{
	public:
		OutlineDrawingPass( GraphicsDeviceInterface& gfx, std::string name )
			:
			RenderQueuePass( std::move( name ) )
		{
			using namespace Bind;
			RegisterSink( DirectBufferSink<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSink( DirectBufferSink<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
			RegisterSource( DirectBufferSource<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSource( DirectBufferSource<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
			AddBind( VertexShader::Resolve( gfx, "./Shaders/Bin/Solid_VS.cso" ) );
			AddBind( PixelShader::Resolve( gfx, "./Shaders/Bin/Solid_PS.cso" ) );
			AddBind( Stencil::Resolve( gfx, Stencil::Mode::Mask ) );
			AddBind( Rasterizer::Resolve( gfx, false ) );
		}
	};
}