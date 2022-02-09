#include "BufferClearPass.h"
#include "../../../Bindable/Bindables/RenderTarget.h"
#include "../../../Bindable/Bindables/DepthStencil.h"
#include "../Sink.h"
#include "../Source.h"

namespace Rgph
{
	BufferClearPass::BufferClearPass( std::string name )
		:
		Pass( std::move( name ) )
	{
		RegisterSink( DirectBufferSink<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
		RegisterSink( DirectBufferSink<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
		RegisterSource( DirectBufferSource<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
		RegisterSource( DirectBufferSource<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
	}

	void BufferClearPass::Execute( GraphicsDeviceInterface& gfx ) const noexcept
	{
		renderTarget->Clear( gfx );
		depthStencil->Clear( gfx );
	}

}