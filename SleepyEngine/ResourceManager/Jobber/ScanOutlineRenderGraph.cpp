#include "ScanOutlineRenderGraph.h"
#include "./Passlib/BufferClearPass.h"
#include "./Passlib/LambertianPass.h"
#include "./Passlib/OutlineDrawingPass.h"
#include "./Passlib/OutlineMaskGenerationPass.h"

ScanOutlineRenderGraph::ScanOutlineRenderGraph( GraphicsDeviceInterface& gfx )
	:
	RenderGraph( gfx )
{
	{
		auto pass = std::make_unique<BufferClearPass>( "clear" );
		pass->SetInputSource( "renderTarget", "$.backbuffer" );
		pass->SetInputSource( "depthStencil", "$.masterDepth" );
		AppendPass( std::move( pass ) );
	}
	{
		auto pass = std::make_unique<LambertianPass>( gfx, "lambertian" );
		pass->SetInputSource( "renderTarget", "clear.renderTarget" );
		pass->SetInputSource( "depthStencil", "clear.depthStencil" );
		AppendPass( std::move( pass ) );
	}
	{
		auto pass = std::make_unique<OutlineMaskGenerationPass>( gfx, "outlineMask" );
		pass->SetInputSource( "depthStencil", "lambertian.depthStencil" );
		AppendPass( std::move( pass ) );
	}
	{
		auto pass = std::make_unique<OutlineDrawingPass>( gfx, "outlineDraw" );
		pass->SetInputSource( "renderTarget", "lambertian.renderTarget" );
		pass->SetInputSource( "depthStencil", "outlineMask.depthStencil" );
		AppendPass( std::move( pass ) );
	}
	SetSinkTarget( "backbuffer", "outlineDraw.renderTarget" );
	Finalize();
}