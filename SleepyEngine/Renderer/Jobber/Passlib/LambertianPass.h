#pragma once
#include "RenderQueuePass.h"
#include "../Job.h"
#include <vector>
#include "../Sink.h"
#include "../Source.h"
#include "../../../Bindable/Bindables/Stencil.h"

class Graphics;

namespace Rgph
{
	class LambertianPass : public RenderQueuePass
	{
	public:
		LambertianPass( Graphics& gfx, std::string name )
			:
			RenderQueuePass( std::move( name ) )
		{
			using namespace Bind; 
			RegisterSink( DirectBufferSink<RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSink( DirectBufferSink<DepthStencil>::Make( "depthStencil", depthStencil ) );
			RegisterSource( DirectBufferSource<RenderTarget>::Make( "renderTarget", renderTarget ) );
			RegisterSource( DirectBufferSource<DepthStencil>::Make( "depthStencil", depthStencil ) );
			AddBind( Stencil::Resolve( gfx, Stencil::Mode::Off ) );
		}
	};
}