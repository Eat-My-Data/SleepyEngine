#include "BufferClearPass.h"
#include "../../../Bindable/Bindables/RenderTarget.h"
#include "../../../Bindable/Bindables/DepthStencil.h"
#include "../PassInput.h"
#include "../PassOutput.h"

namespace Rgph
{
	BufferClearPass::BufferClearPass( std::string name )
		:
		Pass( std::move( name ) )
	{
		RegisterInput( BufferInput<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
		RegisterInput( BufferInput<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
		RegisterOutput( BufferOutput<Bind::RenderTarget>::Make( "renderTarget", renderTarget ) );
		RegisterOutput( BufferOutput<Bind::DepthStencil>::Make( "depthStencil", depthStencil ) );
	}

	void BufferClearPass::Execute( GraphicsDeviceInterface& gfx ) const noexcept
	{
		renderTarget->Clear( gfx );
		depthStencil->Clear( gfx );
	}

}