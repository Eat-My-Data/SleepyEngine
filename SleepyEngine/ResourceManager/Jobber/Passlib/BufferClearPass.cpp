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
		RegisterSink( DirectBufferSink<Bind::BufferResource>::Make( "buffer", buffer ) );
		RegisterSource( DirectBufferSource<Bind::BufferResource>::Make( "buffer", buffer ) );
	}

	void BufferClearPass::Execute( GraphicsDeviceInterface& gfx ) const noexcept
	{
		buffer->Clear( gfx );
	}

}