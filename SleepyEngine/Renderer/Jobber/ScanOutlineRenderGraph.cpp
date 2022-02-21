#include "ScanOutlineRenderGraph.h"
#include "./Passlib/BufferClearPass.h"
#include "./Passlib/LambertianPass.h"
#include "./Passlib/OutlineDrawingPass.h"
#include "./Passlib/OutlineMaskGenerationPass.h"

namespace Rgph
{
	ScanOutlineRenderGraph::ScanOutlineRenderGraph( Graphics& gfx )
		:
		RenderGraph( gfx )
	{
		{

			auto pass = std::make_unique<BufferClearPass>( "clearRT" );
			pass->SetSinkLinkage( "buffer", "$.backbuffer" );
			AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<BufferClearPass>( "clearDS" );
			pass->SetSinkLinkage( "buffer", "$.masterDepth" );
			AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<LambertianPass>( gfx, "lambertian" );
			pass->SetSinkLinkage( "renderTarget", "clearRT.buffer" );
			pass->SetSinkLinkage( "depthStencil", "clearDS.buffer" );
			AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<OutlineMaskGenerationPass>( gfx, "outlineMask" );
			pass->SetSinkLinkage( "depthStencil", "lambertian.depthStencil" );
			AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<OutlineDrawingPass>( gfx, "outlineDraw" );
			pass->SetSinkLinkage( "renderTarget", "lambertian.renderTarget" );
			pass->SetSinkLinkage( "depthStencil", "outlineMask.depthStencil" );
			AppendPass( std::move( pass ) );
		}
		SetSinkTarget( "backbuffer", "outlineDraw.renderTarget" );
		Finalize();
	}
}