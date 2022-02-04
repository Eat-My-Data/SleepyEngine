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
			RegisterInput( BufferInput<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterInput( BufferInput<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
			RegisterOutput( BufferOutput<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterOutput( BufferOutput<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
			AddBind( VertexShader::Resolve( gfx, "./Shaders/Bin/Solid_VS.cso" ) );
			AddBind( PixelShader::Resolve( gfx, "./Shaders/Bin/Solid_PS.cso" ) );
			AddBind( Stencil::Resolve( gfx, Stencil::Mode::Mask ) );
			AddBind( Rasterizer::Resolve( gfx, false ) );
		}
	};
}