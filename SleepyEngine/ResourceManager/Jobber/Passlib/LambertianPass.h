#pragma once
#include "RenderQueuePass.h"
#include "../Job.h"
#include <vector>
#include "../PassInput.h"
#include "../PassOutput.h"
#include "../../../Bindable/Bindables/Stencil.h"

class GraphicsDeviceInterface;

namespace Rgph
{
	class LambertianPass : public RenderQueuePass
	{
	public:
		LambertianPass( GraphicsDeviceInterface& gfx, std::string name )
			:
			RenderQueuePass( std::move( name ) )
		{
			using namespace Bind;
			RegisterInput( BufferInput<RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterInput( BufferInput<DepthStencil>::Make( "depthStencil", depthStencil ) );
			RegisterOutput( BufferOutput<RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterOutput( BufferOutput<DepthStencil>::Make( "depthStencil", depthStencil ) );
			AddBind( Stencil::Resolve( gfx, Stencil::Mode::Off ) );
		}
	};
}